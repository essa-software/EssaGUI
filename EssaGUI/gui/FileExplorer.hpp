#pragma once

#include "Container.hpp"
#include "ListView.hpp"
#include "Textbox.hpp"
#include "ToolWindow.hpp"
#include "Widget.hpp"
#include "WidgetTreeRoot.hpp"
#include <filesystem>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

namespace GUI {

class FileModel : public Model {
public:
    virtual size_t row_count() const override {
        return m_files.size();
    }
    virtual size_t column_count() const override {
        return 5;
    }

    virtual Variant data(size_t row, size_t column) const override;

    virtual Column column(size_t column) const override;

    std::filesystem::path get_path(size_t row) const {
        return m_files[row].path;
    }
    void add_desired_extension(const std::string ext) {
        m_desired_extensions.push_back(ext);
    }

    void update_content(
        std::filesystem::path path, std::function<bool(std::filesystem::path)> condition = [](std::filesystem::path) { return true; });

private:
    static std::string file_type(std::filesystem::path);
    llgl::opengl::Texture const* file_icon(size_t row) const;

    struct File {
        std::filesystem::path path;
        uint64_t size;
        std::filesystem::file_type type;
    };

    std::vector<File> m_files;
    std::vector<std::string> m_desired_extensions;
};

class FileExplorer : public ToolWindow {
public:
    explicit FileExplorer(GUI::Window& wnd);

    enum class FileExplorerType {
        FILE,
        FOLDER
    };

    FileModel* model() { return m_model; }
    FileExplorerType get_type() const { return m_type; }
    void set_type(FileExplorerType type) { m_type = type; }
    void open_path(std::filesystem::path path);

    std::function<void(std::filesystem::path path)> on_submit;

private:
    std::filesystem::path m_current_path;
    Textbox* m_path_textbox {};
    FileModel* m_model {};
    ListView* m_list {};

    FileExplorerType m_type = FileExplorerType::FILE;
};

}
