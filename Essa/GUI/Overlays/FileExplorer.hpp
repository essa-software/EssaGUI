#pragma once

#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/ListView.hpp>
#include <Essa/GUI/Widgets/Textbox.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <filesystem>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

namespace GUI {

class FileModel : public Model {
public:
    virtual size_t children_count(Model::Node const* node) const override {
        return node ? 0 : m_files.size();
    }

    virtual size_t column_count() const override {
        return 5;
    }

    virtual Variant data(Model::Node const& node, size_t column) const override;

    virtual ModelColumn column(size_t column) const override;

    virtual Node child(Node const*, size_t idx) const override {
        return Node { .type = 0, .data = &m_files[idx] };
    }

    std::filesystem::path get_path(size_t row) const {
        return m_files[row].path;
    }
    void add_desired_extension(const std::string ext) {
        m_desired_extensions.push_back(ext);
    }

    void update_content(
        std::filesystem::path path, std::function<bool(std::filesystem::path)> condition = [](std::filesystem::path) { return true; });

private:
    struct File {
        std::filesystem::path path;
        uint64_t size;
        std::filesystem::file_type type;
        bool is_executable;
    };

    static std::string file_type(File const& file);
    llgl::Texture const* file_icon(File const& file) const;

    std::vector<File> m_files;
    std::vector<std::string> m_desired_extensions;
};

class FileExplorer : public ToolWindow {
public:
    explicit FileExplorer(HostWindow& window);

    enum class FileExplorerType {
        FILE,
        FOLDER
    };

    FileModel* model() { return m_model; }
    void open_path(std::filesystem::path path);

    std::function<void(std::filesystem::path path)> on_submit;

    CREATE_VALUE(FileExplorerType, type, FileExplorerType::FILE)
    CREATE_VALUE(std::filesystem::path, current_path, ".")

private:
    Textbox* m_directory_path_textbox {};
    Textbox* m_file_name_textbox {};
    FileModel* m_model {};
    ListView* m_list {};
};

}
