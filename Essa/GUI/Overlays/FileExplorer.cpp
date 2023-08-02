#include "FileExplorer.hpp"
#include "EssaUtil/Config.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Overlays/Prompt.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/TextAlign.hpp>
#include <Essa/GUI/Widgets/Button.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/ListView.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <EssaUtil/UnitDisplay.hpp>
#include <EssaUtil/Units.hpp>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

namespace GUI {

ModelColumn FileModel::column(size_t column) const {
    switch (column) {
    case 0:
        return { .width = 30, .name = "" };
    case 1:
        return { .width = 200, .name = "Name" };
    case 2:
        return { .width = 100, .name = "Size" };
    case 3:
        return { .width = 300, .name = "Modified" };
    case 4:
        return { .width = 150, .name = "File type" };
    }
    return {};
}

Variant FileModel::data(Node row, size_t column) const {
    auto const& file = *static_cast<File const*>(row.data.data);

    switch (column) {
    case 0:
        return file_icon(file);
    case 1:
        return Util::UString { file.path.filename().string() };
    case 2: {
        try {
            return file.type != std::filesystem::file_type::directory ? Util::unit_display(file.size, Util::Quantity::FileSize).to_string()
                                                                      : "";
        } catch (...) {
            return "...";
        }
    }
    case 4: {
        return Util::UString { file_type(file) };
    }
    case 3:
        std::time_t cftime
            = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(std::filesystem::last_write_time(file.path)));
        std::string string = std::asctime(std::localtime(&cftime));
        string.pop_back(); // trailing \n
        return Util::UString { string };
    }
    return "";
}

void FileModel::update_content(std::filesystem::path path, std::function<bool(std::filesystem::path)> condition) {
    m_files.clear();

    for (const auto& o : std::filesystem::directory_iterator(path)) {
        if (!std::filesystem::exists(o) || !condition(o.path().filename()))
            continue;

        bool has_desired_extension = m_desired_extensions.empty();
        for (const auto& e : m_desired_extensions) {
            if (o.path().extension() == e)
                has_desired_extension = true;
        }

        if (!has_desired_extension && !std::filesystem::is_directory(o))
            continue;

        auto size = [&]() -> uintmax_t {
            try {
                return o.is_directory() ? 0 : o.file_size();
            } catch (...) {
                return 0;
            }
        }();

        m_files.push_back(File {
            .path = o.path(),
            .size = size,
            .type = o.status().type(),
            .is_executable
            = o.status().type() == std::filesystem::file_type::regular
                && static_cast<bool>(
                    o.status().permissions()
                    & (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec)
                ),
        });

        // for(const auto& e : m_content.back())
        //     std::cout << e << "\t";
        // std::cout << "\n";
    }

    std::sort(m_files.begin(), m_files.end(), [](File const& a, File const& b) {
        if (a.type == std::filesystem::file_type::directory && b.type != std::filesystem::file_type::directory)
            return true;
        if (b.type == std::filesystem::file_type::directory && a.type != std::filesystem::file_type::directory)
            return false;
        return a.path.filename() < b.path.filename();
    });
}

namespace {

std::string get_regular_file_type_string(FileModel::File const& file) {
    // Some special-cases
    if (file.path.filename() == "CMakeLists.txt")
        return "CMake project";

    std::map<std::string, std::string> extension_to_name {
        { ".cmake", "CMake script" },  { ".essa", "ESSA config" }, { ".md", "Markdown file" },  { ".png", "PNG image" },
        { ".py", "Python script" },    { ".ttf", "TTF font" },     { ".txt", "Text file" },     { ".cpp", "C++ source file" },
        { ".hpp", "C++ header file" }, { ".o", "Object file" },    { ".bf", "Brainfuck file" }, { ".exe", "Executable file" },
    };

    auto extension = file.path.extension().string();
    if (extension.empty() && file.path.filename().string()[0] == '.') {
        // This may be a case for files like .gitignore
        extension = file.path.filename().string();
    }
    auto it = extension_to_name.find(extension);
    if (it == extension_to_name.end()) {
        if (file.is_executable)
            return "Executable file";
        return extension.empty() ? "File" : extension + " file";
    }
    return it->second;
}

}

std::string FileModel::file_type(File const& file) {
    switch (file.type) {
    case std::filesystem::file_type::regular:
        return get_regular_file_type_string(file);
    case std::filesystem::file_type::directory:
        return "Directory";
    case std::filesystem::file_type::symlink:
        // TODO: Resolve this
        return "Symlink";
    case std::filesystem::file_type::block:
        return "Block device";
    case std::filesystem::file_type::character:
        return "Character device";
    case std::filesystem::file_type::fifo:
        return "FIFO";
    case std::filesystem::file_type::socket:
        return "Socket";
    case std::filesystem::file_type::none:
    case std::filesystem::file_type::not_found:
    case std::filesystem::file_type::unknown:
        return "???";
    }
    ESSA_UNREACHABLE;
}

llgl::Texture const* FileModel::file_icon(File const& file) const {
    static llgl::Texture& directory_icon = GUI::Application::the().resource_manager().require_texture("gui/directory.png");
    static llgl::Texture& regular_file_icon = GUI::Application::the().resource_manager().require_texture("gui/regularFile.png");
    static llgl::Texture& block_device_icon = GUI::Application::the().resource_manager().require_texture("gui/blockDevice.png");
    static llgl::Texture& symlink_icon = GUI::Application::the().resource_manager().require_texture("gui/symlink.png");
    static llgl::Texture& socket_icon = GUI::Application::the().resource_manager().require_texture("gui/socket.png");
    static llgl::Texture& executable_file_icon = GUI::Application::the().resource_manager().require_texture("gui/executableFile.png");

    switch (file.type) {
    case std::filesystem::file_type::directory:
        return &directory_icon;
    case std::filesystem::file_type::block:
        return &block_device_icon;
    case std::filesystem::file_type::symlink:
        return &symlink_icon;
    case std::filesystem::file_type::socket:
        return &socket_icon;
    default:
        if (file.is_executable)
            return &executable_file_icon;
        return &regular_file_icon;
    }
}

FileExplorer::FileExplorer(WidgetTreeRoot& window)
    : WindowRoot(window) {
    (void)load_from_eml_resource(Application::the().resource_manager().require<EML::EMLResource>("FileExplorer.eml"));

    auto container = static_cast<Container*>(main_widget());

    m_list = container->find_widget_of_type_by_id_recursively<ListView>("list");
    m_list->on_click = [&](unsigned row) {
        auto path = m_model->get_path(row);

        if (m_type == FileExplorerType::FILE && !std::filesystem::is_directory(path)) {
            if (on_submit)
                on_submit(path);
            close();
        }
        else {
            open_path(path);
        }
    };

    m_model = &m_list->create_and_set_model<FileModel>();

    m_directory_path_textbox = container->find_widget_of_type_by_id_recursively<Textbox>("directory_path");
    m_directory_path_textbox->set_type(Textbox::TEXT);
    m_directory_path_textbox->on_enter = [this](Util::UString const& str) { open_path(str.encode()); };

    // m_file_name_textbox = container->find_widget_of_type_by_id_recursively<Textbox>("file_name");
    // m_file_name_textbox->set_type(Textbox::TEXT);

    auto search_textbox = container->find_widget_of_type_by_id_recursively<Textbox>("search");
    search_textbox->set_type(Textbox::TEXT);
    search_textbox->on_change = [this](Util::UString const& query) {
        // FIXME: Port this to UString instead of encoding this to std::string
        //        because of lazyness.
        m_model->update_content(m_current_path, [query = query.encode()](std::filesystem::path path) -> bool {
            // TODO: Support fuzzy search
            auto str = path.string();
            auto size = query.size();

            if (query[0] != '*' && query[size - 1] != '*') {
                return str.substr(0, size) == query;
            }
            else if (query[0] != '*' && query[size - 1] == '*') {
                return str.substr(0, size - 1) == query.substr(0, size - 1);
            }
            else if (query[0] == '*' && query[size - 1] != '*') {
                for (unsigned i = 0; i < str.size(); i++) {
                    if (str.substr(i, std::min(i + size - 1, str.size())) == query.substr(1, size))
                        return true;
                }
                return false;
            }
            else {
                for (unsigned i = 0; i < str.size(); i++) {
                    if (str.substr(i, std::min(i + size - 1, str.size())) == query.substr(1, size - 1))
                        return true;
                }
                return false;
            }
            return false;
        });
    };

    auto parent_directory_button = container->find_widget_of_type_by_id_recursively<TextButton>("parent_directory");
    parent_directory_button->on_click = [&]() { open_path(m_current_path.parent_path()); };

    auto create_directory_button = container->find_widget_of_type_by_id_recursively<TextButton>("create_directory");
    create_directory_button->on_click = [&]() {
        // FIXME: Taking HostWindow from arbitrary widget. This won't be needed after
        //        Dialog refactoring is finished.
        auto path = GUI::prompt(create_directory_button->host_window(), "Folder name: ", "Create folder");
        if (path.has_value()) {
            try {
                // C++ Why mutable paths?!!!
                auto new_path = m_current_path;
                new_path.append(path->encode());
                std::filesystem::create_directory(new_path);
                m_model->update_content(m_current_path);
            } catch (std::filesystem::filesystem_error& error) {
                // FIXME: Taking HostWindow from arbitrary widget. This won't be needed after
                //        Dialog refactoring is finished.
                GUI::message_box(
                    create_directory_button->host_window(), Util::UString { error.what() }, "Error", GUI::MessageBox::Buttons::Ok
                );
            }
        };
    };

    auto sidebar = container->find_widget_of_type_by_id_recursively<GUI::Container>("sidebar");
    sidebar->set_background_color(sidebar->theme().sidebar);

    auto add_common_location = [&](Util::UString const& name, std::filesystem::path path) {
        auto button = sidebar->add_widget<GUI::TextButton>();
        button->set_content(name);
        button->set_tooltip_text(Util::UString { path.string() });
        button->on_click = [this, path]() { open_path(path); };
    };

    add_common_location("Root", "/");
    auto home_directory = getenv("HOME");
    if (home_directory)
        add_common_location("Home", home_directory);

    open_path(std::filesystem::current_path());
}

void FileExplorer::open_path(std::filesystem::path path) {

    if (!std::filesystem::is_directory(path)) {
        // TODO: Implement that
        std::cout << "select path: " << path << std::endl;
        return;
    }
    path = std::filesystem::absolute(path).lexically_normal();

    // FIXME: Hack because std c++ is weird as always with differentiating
    //        "/foo/bar" and "/foo/bar/" and (even weirder) considers "/foo/bar"
    //        a PARENT for "/foo/bar/". WTF.
    if (path.string().ends_with("/")) {
        path = path.parent_path();
    }

    try {
        m_model->update_content(path);
    } catch (std::filesystem::filesystem_error& error) {
        m_model->update_content(m_current_path);
        // FIXME: Taking HostWindow from arbitrary widget. This won't be needed after
        //        Dialog refactoring is finished.
        GUI::message_box(
            m_list->host_window(), Util::UString { error.path1().string() + ": " + error.code().message() }, "Error!",
            GUI::MessageBox::Buttons::Ok
        );
        return;
    }
    m_current_path = path;
    m_directory_path_textbox->set_content(Util::UString { path.string() }, NotifyUser::No);
    m_list->set_scroll({});
}

std::optional<std::filesystem::path> FileExplorer::get_path_to_open(HostWindow& host_window) {
    auto explorer = host_window.open_overlay<FileExplorer>();
    std::optional<std::filesystem::path> result;
    explorer.overlay.center_on_screen();
    explorer.window_root.on_submit = [&](const std::filesystem::path& path) { result = path; };
    explorer.overlay.show_modal();
    return result;
}

}
