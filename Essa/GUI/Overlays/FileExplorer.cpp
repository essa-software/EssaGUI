#include "FileExplorer.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/VectorImage.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Overlays/MessageBox.hpp>
#include <Essa/GUI/Overlays/Prompt.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/TextAlign.hpp>
#include <Essa/GUI/Widgets/Button.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/UnitDisplay.hpp>
#include <EssaUtil/Units.hpp>

#include <filesystem>
#include <fmt/chrono.h>
#include <functional>
#include <iostream>
#include <map>
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
    case 3: {
        using namespace std::chrono;
        return Util::UString { std::format("{:%Y-%m-%d %H:%M:%S}", time_point_cast<seconds>(file.last_modified)) };
    }
    case 4: {
        return Util::UString { file_type(file) };
    }
    }
    return "";
}

void FileModel::update_content(std::filesystem::path path, std::function<bool(std::filesystem::path)> condition) {
    m_files.clear();

    for (auto const& o : std::filesystem::directory_iterator(path)) {
        if (!std::filesystem::exists(o) || !condition(o.path().filename()))
            continue;

        bool has_desired_extension = m_desired_extensions.empty();
        for (auto const& e : m_desired_extensions) {
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
            .last_modified = std::filesystem::last_write_time(o.path()),
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
    auto load = [](std::string const& path) -> llgl::Texture const& {
        auto const& vector_image = GUI::Application::the().resource_manager().require_icon("essa/gui/file_types/" + path);
        return vector_image.render({ 16, 16 });
    };

    static llgl::Texture const& directory_icon = load("directory.tvg");
    static llgl::Texture const& regular_file_icon = load("regular_file.tvg");
    static llgl::Texture const& block_device_icon = load("block_device.tvg");
    static llgl::Texture const& symlink_icon = load("symlink.tvg");
    static llgl::Texture const& socket_icon = load("socket.tvg");
    static llgl::Texture const& executable_file_icon = load("executable.tvg");

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

FileExplorer::FileExplorer(WidgetTreeRoot& window, Mode mode)
    : WindowRoot(window) {
    (void)load_from_eml_resource(Application::the().resource_manager().require<EML::EMLResource>("FileExplorer.eml"));

    auto container = static_cast<Container*>(main_widget());

    auto submit_save = [this]() {
        auto path = m_current_path / m_file_name_textbox->content().encode();
        if (std::filesystem::exists(path)) {
            auto result = GUI::message_box(
                &this->window().host_window(), "File already exists. Overwrite?", "Warning",
                {
                    .buttons = GUI::MessageBox::Buttons::YesNo,
                    .icon = GUI::MessageBox::Icon::Warning,
                }
            );
            if (result == GUI::MessageBox::ButtonRole::No) {
                return;
            }
        }
        on_submit(path);
        close();
    };

    m_list = container->find_widget_of_type_by_id_recursively<TreeView>("list");
    m_list->on_click = [mode, this](Model::NodeData row) {
        if (mode == Mode::Save) {
            auto const* data = static_cast<FileModel::File const*>(row.data);
            auto path = data->path;
            m_file_name_textbox->set_content(Util::UString { path.filename().string() }, NotifyUser::No);
        }
    };
    m_list->on_double_click = [submit_save, mode, this](Model::NodeData row) {
        auto const* data = static_cast<FileModel::File const*>(row.data);
        auto path = data->path;

        if (m_type == FileType::File && !std::filesystem::is_directory(path)) {
            if (mode == Mode::Save) {
                submit_save();
            }
            else {
                if (on_submit)
                    on_submit(path);
                close();
            }
        }
        else {
            open_path(path);
        }
    };
    m_list->on_context_menu_request = [this](Model::NodeData row) -> std::optional<ContextMenu> {
        auto const* data = static_cast<FileModel::File const*>(row.data);
        auto path = data->path;
        ContextMenu menu;
        menu.set_title(Util::UString(path.filename().string()));
        menu.add_action("Open", [this, path]() { open_path(path.filename()); });
        return menu;
    };

    m_model = &m_list->create_and_set_model<FileModel>();

    m_file_name_textbox = container->find_widget_of_type_by_id_recursively<Textbox>("file_name");
    if (mode == Mode::Open) {
        m_file_name_textbox->set_visible(false);
    }
    else {
        m_file_name_textbox->set_type(Textbox::TEXT);
        m_file_name_textbox->on_enter = [this, submit_save](Util::UString const& str) {
            if (str.find("/").has_value()) {
                open_path(str.encode());
            }
            else {
                submit_save();
            }
        };
    }

    m_directory_path_textbox = container->find_widget_of_type_by_id_recursively<Textbox>("directory_path");
    m_directory_path_textbox->set_type(Textbox::TEXT);
    m_directory_path_textbox->on_enter = [this](Util::UString const& str) { open_path(str.encode()); };

    auto* submit_button = &container->find<TextButton>("submit");
    if (mode == Mode::Save) {
        submit_button->set_content("Save");
        m_list->on_click = [this](Model::NodeData node) {
            auto const& file = *static_cast<FileModel::File const*>(node.data);
            m_file_name_textbox->set_content(Util::UString { file.path.filename().string() }, NotifyUser::No);
        };
        submit_button->on_click = [submit_save]() { submit_save(); };
    }
    else {
        submit_button->on_click = [this]() {
            auto focused_node = m_list->focused_node();
            if (!focused_node) {
                return;
            }
            auto const& file = *static_cast<FileModel::File const*>(focused_node->data);

            bool submit = false;
            if (m_type == FileType::Directory) {
                if (file.type == std::filesystem::file_type::directory) {
                    submit = true;
                }
            }
            else {
                if (file.type == std::filesystem::file_type::directory) {
                    open_path(file.path);
                }
                else {
                    submit = true;
                }
            }
            if (submit) {
                on_submit(file.path);
                close();
            }
        };
    }

    auto* search_textbox = container->find_widget_of_type_by_id_recursively<Textbox>("search");
    search_textbox->set_type(Textbox::TEXT);
    search_textbox->on_change = [this](Util::UString const& query) {
        // FIXME: Port this to UString instead of encoding this to std::string
        //        because of lazyness.
        try {
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
        } catch (std::filesystem::filesystem_error& error) {
            GUI::message_box(
                &this->window().host_window(), Util::UString { error.what() }, "Error",
                {
                    .buttons = GUI::MessageBox::Buttons::Ok,
                    .icon = GUI::MessageBox::Icon::Error,
                }
            );
        }
    };

    auto* parent_directory_button = container->find_widget_of_type_by_id_recursively<TextButton>("parent_directory");
    parent_directory_button->on_click = [&]() { open_path(m_current_path.parent_path()); };

    auto* create_directory_button = container->find_widget_of_type_by_id_recursively<TextButton>("create_directory");
    create_directory_button->on_click = [create_directory_button, this]() {
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
                GUI::message_box(
                    &this->window().host_window(), Util::UString { error.what() }, "Error",
                    {
                        .buttons = GUI::MessageBox::Buttons::Ok,
                        .icon = GUI::MessageBox::Icon::Error,
                    }
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
        button->set_size({ Util::Length::Auto, Util::Length::Initial });
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
        GUI::message_box(
            &this->window().host_window(), Util::UString { error.path1().string() + ": " + error.code().message() }, "Error",
            {
                .buttons = GUI::MessageBox::Buttons::Ok,
                .icon = GUI::MessageBox::Icon::Error,
            }
        );
        return;
    }
    m_current_path = path;
    m_directory_path_textbox->set_content(Util::UString { path.string() }, NotifyUser::No);
    m_list->set_scroll({});
    m_list->focus({});
}

std::optional<std::filesystem::path> FileExplorer::get_path_to_open(HostWindow* window) {
    auto explorer = GUI::Application::the().open_host_window<FileExplorer>();
    std::optional<std::filesystem::path> result;
    explorer.window.center_on_screen();
    explorer.root.on_submit = [&](std::filesystem::path const& path) { result = path; };
    explorer.window.show_modal(window);
    return result;
}

std::optional<std::filesystem::path> FileExplorer::get_path_to_save(HostWindow* window) {
    auto explorer = GUI::Application::the().open_host_window<FileExplorer>(FileExplorer::Mode::Save);
    std::optional<std::filesystem::path> result;
    explorer.window.center_on_screen();
    explorer.root.on_submit = [&](std::filesystem::path const& path) { result = path; };
    explorer.window.show_modal(window);
    return result;
}

std::optional<std::filesystem::path> FileExplorer::get_directory_to_open(HostWindow* window) {
    auto explorer = GUI::Application::the().open_host_window<FileExplorer>();
    std::optional<std::filesystem::path> result;
    explorer.window.center_on_screen();
    explorer.root.set_type(FileExplorer::FileType::Directory);
    explorer.root.on_submit = [&](std::filesystem::path const& path) { result = path; };
    explorer.window.show_modal(window);
    return result;
}
}
