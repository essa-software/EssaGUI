#include "EMLObject.hpp"

#include "AST.hpp"
#include "Lexer.hpp"
#include "Loader.hpp"
#include "Parser.hpp"

#include <EssaUtil/Config.hpp>
#include <EssaUtil/GenericParser.hpp>
#include <EssaUtil/Stream/File.hpp>
#include <EssaUtil/Stream/MemoryStream.hpp>

namespace EML {

bool EMLObject::load_from_eml_resource(EMLResource const& resource) {
    return load_from_eml_string(resource.source);
}

bool EMLObject::load_from_eml_string(std::string_view source) {
    auto input = Util::ReadableMemoryStream::from_string(source);
    return load_from_eml_stream(input);
}

bool EMLObject::load_from_eml_file(std::string const& path) {
    auto input = MUST(Util::ReadableFileStream::open(path));
    return load_from_eml_stream(input);
}

bool EMLObject::load_from_eml_stream(Util::ReadableStream& input) {
    EML::Lexer lexer { input };
    auto tokens = MUST(lexer.lex());
    EML::Parser parser { tokens };
    auto maybe_object = parser.parse_object();
    if (maybe_object.is_error()) {
        fmt::print("EMLObject: ParseError: {}\n", maybe_object.release_error().message);
        return false;
    }
    auto object = maybe_object.release_value();

    EML::Loader loader;
    loader.push_scope(&object);
    auto maybe_error = load_from_eml_object(object, loader);
    if (maybe_error.is_error()) {
        fmt::print("EMLObject: EMLError: {}\n", maybe_error.release_error().message);
        return false;
    }
    loader.pop_scope();
    return true;
}

}
