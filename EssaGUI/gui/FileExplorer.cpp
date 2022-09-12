#include "FileExplorer.hpp"

#include "Application.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include "ListView.hpp"
#include "MessageBox.hpp"
#include "NotifyUser.hpp"
#include "Prompt.hpp"
#include "TextButton.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToolWindow.hpp"

#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaUtil/UnitDisplay.hpp>

#include <EssaUtil/Units.hpp>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace GUI {

Model::Column FileModel::column(size_t column) const {
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

Variant FileModel::data(size_t row, size_t column) const {
    auto const& file = m_files[row];

    switch (column) {
    case 0:
        return file_icon(row);
    case 1:
        return Util::UString { file.path.filename().string() };
    case 2: {
        try {
            return file.type != std::filesystem::file_type::directory
                ? Util::unit_display(file.size, Util::Quantity::FileSize).to_string()
                : "";
        } catch (...) {
            return "...";
        }
    }
    case 4: {
        return Util::UString { file_type(file) };
    }
    case 3:
        std::time_t cftime = std::chrono::system_clock::to_time_t(
            std::chrono::file_clock::to_sys(std::filesystem::last_write_time(file.path)));
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
            .is_executable = o.status().type() == std::filesystem::file_type::regular && static_cast<bool>(o.status().permissions() & (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec)),
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

std::string FileModel::file_type(File const& file) {
    if (file.is_executable)
        return "Executable file";

    // Some special-cases
    if (file.path.filename() == "CMakeLists.txt")
        return "CMake project";

    std::map<std::string, std::string> extension_to_name {
        { ".cmake", "CMake script" },
        { ".essa", "ESSA config" },
        { ".md", "Markdown file" },
        { ".png", "PNG image" },
        { ".py", "Python script" },
        { ".ttf", "TTF font" },
        { ".txt", "Text file" },
        { ".cpp", "C++ source file" },
        { ".hpp", "C++ header file" },
        { ".o", "Object file" },
        { ".bf", "Brainfuck file" },
        { ".exe", "Executable file" },
    };

    if (file.type == std::filesystem::file_type::directory)
        return "Directory";

    auto extension = file.path.extension().string();
    if (extension.empty()) {
        if (file.path.filename().string()[0] == '.') {
            // This may be a case for files like .gitignore
            extension = file.path.filename().string();
        }
        else {
            return "File";
        }
    }
    auto it = extension_to_name.find(extension);
    if (it == extension_to_name.end())
        return extension + " file";
    return it->second;
}

llgl::opengl::Texture const* FileModel::file_icon(size_t row) const {
    static llgl::opengl::Texture& directory_icon = GUI::Application::the().resource_manager().require_texture("gui/directory.png");
    static llgl::opengl::Texture& regular_file_icon = GUI::Application::the().resource_manager().require_texture("gui/regularFile.png");
    static llgl::opengl::Texture& block_device_icon = GUI::Application::the().resource_manager().require_texture("gui/blockDevice.png");
    static llgl::opengl::Texture& symlink_icon = GUI::Application::the().resource_manager().require_texture("gui/symlink.png");
    static llgl::opengl::Texture& socket_icon = GUI::Application::the().resource_manager().require_texture("gui/socket.png");
    static llgl::opengl::Texture& executable_file_icon = GUI::Application::the().resource_manager().require_texture("gui/executableFile.png");

    switch (m_files[row].type) {
    case std::filesystem::file_type::directory:
        return &directory_icon;
    case std::filesystem::file_type::block:
        return &block_device_icon;
    case std::filesystem::file_type::symlink:
        return &symlink_icon;
    case std::filesystem::file_type::socket:
        return &socket_icon;
    default:
        if (m_files[row].is_executable)
            return &executable_file_icon;
        return &regular_file_icon;
    }
}

FileExplorer::FileExplorer(HostWindow& window)
    : ToolWindow(window) {
    static llgl::opengl::Texture& parent_directory_icon = resource_manager().require_texture("gui/parentDirectory.png");
    static llgl::opengl::Texture& new_folder_icon = resource_manager().require_texture("gui/newFolder.png");
    static llgl::opengl::Texture& new_file_icon = resource_manager().require_texture("gui/newFile.png");

    auto& container = set_main_widget<GUI::Container>();
    container.set_layout<VerticalBoxLayout>().set_spacing(1);

    Length toolbar_height = { static_cast<float>(theme().line_height), Length::Unit::Px };

    auto toolbar = container.add_widget<Container>();
    toolbar->set_layout<HorizontalBoxLayout>();
    toolbar->set_size({ Length::Auto, toolbar_height });

    m_path_textbox = toolbar->add_widget<Textbox>();
    m_path_textbox->set_type(Textbox::Type::TEXT);
    m_path_textbox->set_content(Util::UString { m_current_path.string() });

    auto parent_directory_button = toolbar->add_widget<TextButton>();
    parent_directory_button->set_image(&parent_directory_icon);
    parent_directory_button->set_tooltip_text("Parent");
    parent_directory_button->set_alignment(Align::Center);
    parent_directory_button->set_size({ toolbar_height, Length::Auto });

    auto create_directory_button = toolbar->add_widget<TextButton>();
    create_directory_button->set_image(&new_folder_icon);
    create_directory_button->set_tooltip_text("Create folder");
    create_directory_button->set_alignment(Align::Center);
    create_directory_button->set_size({ toolbar_height, Length::Auto });
    create_directory_button->on_click = [&]() {
        auto path = GUI::prompt(host_window(), "Folder name: ", "Create folder");
        if (path.has_value()) {
            try {
                // C++ Why mutable paths?!!!
                auto new_path = m_current_path;
                new_path.append(path->encode());
                std::filesystem::create_directory(new_path);
                m_model->update_content(m_current_path);
            } catch (std::filesystem::filesystem_error& error) {
                GUI::message_box(host_window(), Util::UString { error.what() }, "Error", GUI::MessageBox::Buttons::Ok);
            }
        };
    };

    auto create_file_button = toolbar->add_widget<TextButton>();
    create_file_button->set_image(&new_file_icon);
    create_file_button->set_tooltip_text("Create file");
    create_file_button->set_alignment(Align::Center);
    create_file_button->set_size({ 30.0_px, Length::Auto });
    create_file_button->on_click = [&]() {
        auto file_name = GUI::prompt(host_window(), "File name with extension: ", "Create file");
        if (file_name.has_value()) {
            // C++ Why mutable paths?!!!
            auto new_path = m_current_path;
            new_path.append(file_name->encode());
            std::ofstream f_out(new_path);
            m_model->update_content(m_current_path);
        };
    };

    auto find = toolbar->add_widget<Textbox>();
    find->set_placeholder("Find file or directory");
    find->set_size({ { 25.0, Length::Percent }, Length::Auto });
    find->set_type(Textbox::Type::TEXT);
    find->on_change = [&](Util::UString const& content) {
        // FIXME: The encode() hack
        m_model->update_content(m_current_path, [content = content.encode()](std::filesystem::path path) {
            // TODO: Support fuzzy search
            auto str = path.string();
            auto size = content.size();

            if (content[0] != '*' && content[size - 1] != '*') {
                return str.substr(0, size) == content;
            }
            else if (content[0] != '*' && content[size - 1] == '*') {
                return str.substr(0, size - 1) == content.substr(0, size - 1);
            }
            else if (content[0] == '*' && content[size - 1] != '*') {
                for (unsigned i = 0; i < str.size(); i++) {
                    if (str.substr(i, std::min(i + size - 1, str.size())) == content.substr(1, size))
                        return true;
                }
                return false;
            }
            else {
                for (unsigned i = 0; i < str.size(); i++) {
                    if (str.substr(i, std::min(i + size - 1, str.size())) == content.substr(1, size - 1))
                        return true;
                }
                return false;
            }
            return false;
        });
    };

    auto main_container = container.add_widget<Container>();
    main_container->set_layout<HorizontalBoxLayout>().set_spacing(1);

    auto sidebar = main_container->add_widget<Container>();
    sidebar->set_size({ { 20.0, Length::Percent }, Length::Auto });
    sidebar->set_layout<GUI::VerticalBoxLayout>();
    sidebar->set_background_color(theme().sidebar);

    auto add_common_location = [&](Util::UString const& name, std::filesystem::path path) {
        auto button = sidebar->add_widget<GUI::TextButton>();
        button->set_content(name);
        button->set_tooltip_text(Util::UString { path.string() });
        button->on_click = [this, path]() {
            open_path(path);
        };
    };
    add_common_location("Root", "/");
    auto home_directory = getenv("HOME");
    if (home_directory)
        add_common_location("Home", home_directory);

    m_list = main_container->add_widget<ListView>();
    m_model = &m_list->create_and_set_model<FileModel>();

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

    m_list->on_context_menu_request = [&](unsigned row) -> std::optional<ContextMenu> {
        ContextMenu context_menu;
        context_menu.set_title({ m_model->get_path(row).filename().string() });
        context_menu.add_action("Open", [this, row]() {
            m_list->on_click(row);
        });
        return context_menu;
    };

    m_path_textbox->on_enter = [&](Util::UString const& path) {
        open_path(path.encode());
    };

    parent_directory_button->on_click = [&]() {
        open_path(m_current_path.parent_path());
    };

    open_path("../worlds");

    auto open_folder_container = container.add_widget<GUI::Container>();
    open_folder_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    open_folder_container->set_size({ Length::Auto, 40.0_px });
    open_folder_container->set_background_color(theme().sidebar);
    auto footer_text = open_folder_container->add_widget<GUI::Textfield>();
    footer_text->set_content("Choose folder to open: ");
    footer_text->set_alignment(GUI::Align::CenterRight);
    footer_text->set_size({ { 70.0, Length::Percent }, Length::Auto });

    auto open_directory_btn = open_folder_container->add_widget<GUI::TextButton>();
    open_directory_btn->set_content("Open folder");
    open_directory_btn->set_alignment(Align::Center);
    open_directory_btn->set_background_color(Util::Colors::Red);
}

void FileExplorer::open_path(std::filesystem::path path) {
    if (!std::filesystem::is_directory(path)) {
        // TODO: Implement that
        std::cout << "select path: " << path << std::endl;
        return;
    }
    path = std::filesystem::absolute(path).lexically_normal();
    try {
        m_model->update_content(path);
    } catch (std::filesystem::filesystem_error& error) {
        m_model->update_content(m_current_path);
        GUI::message_box(host_window(), Util::UString { error.path1().string() + ": " + error.code().message() }, "Error!", GUI::MessageBox::Buttons::Ok);
        return;
    }
    m_current_path = path;
    m_path_textbox->set_content(Util::UString { path.string() }, NotifyUser::No);
    m_list->set_scroll(0);
}

}
