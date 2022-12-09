#include "LLGL/OpenGL/Texture.hpp"
#include <EssaGUI/Application.hpp>
#include <EssaGUI/Model.hpp>
#include <EssaGUI/Widgets/TreeView.hpp>
#include <EssaUtil/Config.hpp>
#include <cassert>
#include <fmt/core.h>

struct Column {
    std::string name;
    std::string type;
};

struct Index {
    std::string type;
    std::string name;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
    std::vector<Index> indexes;
};

struct Database {
    std::vector<Table> m_tables;
};

class VariantList {
public:
    VariantList(std::initializer_list<GUI::Variant> list)
        : m_list(list) { }

    auto operator[](size_t idx) const { return m_list.begin()[idx]; }

private:
    std::initializer_list<GUI::Variant> m_list;
};

class DatabaseModel : public GUI::Model {
public:
    Database db;

private:
    static constexpr int T_Table = 0;
    static constexpr int T_ColumnList = 1;
    static constexpr int T_IndexList = 2;
    static constexpr int T_Column = 3;
    static constexpr int T_Index = 4;

    virtual size_t column_count() const override {
        return 2;
    }

    virtual GUI::ModelColumn column(size_t column) const override {
        switch (column) {
        case 0:
            return { .width = 250, .name = "name" };
        case 1:
            return { .width = 200, .name = "data" };
        }
        ESSA_UNREACHABLE;
    }

    virtual GUI::Variant data(Node const& node, size_t column) const override {
        switch (node.type) {
        case T_Table:
            return VariantList { Util::UString { static_cast<Table const*>(node.data)->name }, "" }[column];
        case T_ColumnList:
            return VariantList { "Columns", "" }[column];
        case T_IndexList:
            return VariantList { "Indexes", "" }[column];
        case T_Column: {
            auto data = static_cast<Column const*>(node.data);
            return VariantList { Util::UString { data->name }, Util::UString { data->type } }[column];
        }
        case T_Index: {
            auto data = static_cast<Index const*>(node.data);
            return VariantList { Util::UString { data->name }, Util::UString { data->type } }[column];
        }
        }
        ESSA_UNREACHABLE;
    }

    virtual llgl::Texture const* icon(Node const& node) const override {
        switch (node.type) {
        case T_Table:
            return &GUI::Application::the().resource_manager().require_texture("gui/blockDevice.png");
        case T_Column:
            return &GUI::Application::the().resource_manager().require_texture("gui/executableFile.png");
        case T_Index:
            return &GUI::Application::the().resource_manager().require_texture("gui/symlink.png");
        default:
            return nullptr;
        }
    }

    virtual size_t children_count(Node const* node) const override {
        if (!node) {
            return db.m_tables.size();
        }
        switch (node->type) {
        case T_Table:
            return 2; // "Columns" and "Indexes"
        case T_ColumnList:
            return static_cast<Table const*>(node->data)->columns.size();
        case T_IndexList:
            return static_cast<Table const*>(node->data)->indexes.size();
        case T_Column: {
            return 0;
        }
        case T_Index: {
            return 0;
        }
        }
        ESSA_UNREACHABLE;
    }

    virtual Node child(Node const* node, size_t idx) const override {
        if (!node) {
            return Node { .type = T_Table, .data = &db.m_tables[idx] };
        }
        switch (node->type) {
        case T_Table:
            return Node { .type = idx == 0 ? T_ColumnList : T_IndexList, .data = node->data };
        case T_ColumnList:
            return Node { .type = T_Column, .data = &static_cast<Table const*>(node->data)->columns[idx] };
        case T_IndexList:
            return Node { .type = T_Index, .data = &static_cast<Table const*>(node->data)->indexes[idx] };
        }
        ESSA_UNREACHABLE;
    }
};

std::string indent(size_t d) {
    std::string result;
    result.resize(d * 2, ' ');
    return result;
}

void print_children(size_t depth, GUI::Model const& model, GUI::Model::Node const* node) {
    for (size_t s = 0; s < model.children_count(node); s++) {
        fmt::print("{}", indent(depth));
        auto child = model.child(node, s);
        for (size_t c = 0; c < model.column_count(); c++) {
            fmt::print("{}; ", std::get<Util::UString>(model.data(child, c)).encode());
        }
        fmt::print("\n");

        print_children(depth + 1, model, &child);
    }
}

void print_model(GUI::Model const& model) {
    for (size_t c = 0; c < model.column_count(); c++) {
        auto column = model.column(c);
        fmt::print("'{}' ({}px); ", column.name.encode(), column.width);
    }
    fmt::print("\n");

    print_children(0, model, nullptr);
}

void print_model_flattened(GUI::TreeView const& view) {
    auto row_count = view.displayed_row_count();
    for (size_t s = 0; s < row_count; s++) {
        auto node = view.displayed_row_at_index(s);
        fmt::print("{}: {}\n", fmt::join(node.first, ", "), node.second.data ? std::get<Util::UString>(view.model().data(node.second, 0)).encode() : "NULL");
    }
}

int main() {
    auto db_model = std::make_unique<DatabaseModel>();

    Table t1;
    t1.name = "olejki";
    t1.columns.push_back({ .name = "idolejku", .type = "int" });
    t1.columns.push_back({ .name = "nazwaolejku", .type = "varchar" });
    t1.indexes.push_back({ .type = "pri", .name = "idolejku" });

    Table t2;
    t2.name = "test";
    t2.columns.push_back({ .name = "testid", .type = "int" });
    t2.indexes.push_back({ .type = "pri", .name = "testid" });
    t2.indexes.push_back({ .type = "uni", .name = "testid but not needed" });

    db_model->db.m_tables.push_back(t1);
    db_model->db.m_tables.push_back(t2);

    print_model(*db_model);

    GUI::Application app;
    auto& window = app.create_host_window({ 500, 500 }, "TreeView");
    auto& tv = window.set_main_widget<GUI::TreeView>();

    tv.set_model(std::move(db_model));
    tv.expand({ 0 });
    tv.expand({ 0, 1 });
    tv.expand({ 1 });
    print_model_flattened(tv);
    tv.set_display_header(false);

    app.run();
    return 0;
}
