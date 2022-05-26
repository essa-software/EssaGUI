#pragma once

#include "Container.hpp"
#include "ListView.hpp"
#include "Textbox.hpp"
#include "ToolWindow.hpp"
#include "Widget.hpp"
#include "WidgetTreeRoot.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <filesystem>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

namespace GUI {

class FileModel : public Model {
public:
    virtual size_t row_count() const override {
        return m_content.size();
    }
    virtual size_t column_count() const override {
        return 4;
    }
    virtual std::string data(size_t row, size_t column) const override {
        return m_content[row][column];
    }

    virtual Column column(size_t column) const override;

    std::filesystem::path get_path(size_t row) const {
        return m_paths[row];
    }
    void add_desired_extension(const std::string ext) {
        m_extensions.push_back(ext);
    }

    void update_content(
        std::filesystem::path path, std::function<bool(std::filesystem::path)> condition = [](std::filesystem::path) { return true; });

private:
    static std::string file_type(std::filesystem::path);

    std::vector<std::vector<std::string>> m_content;
    std::vector<std::filesystem::path> m_paths;
    std::vector<std::string> m_extensions;
};

class FileExplorer : public ToolWindow {
public:
    explicit FileExplorer(GUI::SFMLWindow& wnd);

    enum class FileExplorerType {
        FILE,
        FOLDER
    };

    FileModel* model() { return m_model; }
    void type(FileExplorerType type) { m_type = type; }
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
