# EML - ESSA Markup Language

**EML** is a declarative language for defining GUI application layout. It is based on SerenityOS's [GML](http://man.serenityos.org/man5/GML.html), which is in turn inspired by Qt's [QML](https://doc.qt.io/qt-6/qml-tutorial.html).

The example EML code looks like this:

```qml
// root object
@Application {
    main_widget: @Container { // object property
        layout: @VerticalBoxLayout {
            padding: 5 // numeric property
        }

        // child widgets
        @Textfield {
            height: auto
            content: "Click the button"
        }

        // child widget with id
        @Button submit {
            content: "test"
        }
    }
}
```

Result:

![result](./eml-example.png)

The main changes made to GML include:

-   Possibility of defining widget's ID (name) just after class name, like Button in example
-   Possibility of defining [custom types](#custom-types)
-   New value types: `length` (instead of Serenity's GUI::UIDimension), `resource_id` and `range`

## Objects

The main building block of EML code is **object**. EML objects correspond to actual C++ objects that inherit from `EML::EMLObject` (see [implementing loaders]())

EML objects consists of:

-   class name, which determines the C++ object that will be created
-   (_optional_) object ID
-   properties, defined as key + value (e.g `layout`, `width`) (see [value types](#value-types))

## Value types

### `double`

Numeric value, corresponds to C++ `double` type.

### `bool`

Logical value, can be `true` or `false`.

```qml
enabled: true
toggleable: false
```

### `string`

A Unicode string.

```qml
content: "some string"
```

### `object`

Any EML object.

```qml
layout: @HorizontalBoxLayout
```

```qml
main_widget: @Button {
    content: "test"
}
```

### `length`

Widget size component with unit.

```qml
// Fixed size. Corresponds to `50.0_perc` or `Length{50, Length::Unit::Perc}` in C++.
width: 100px
// Fixed size. Corresponds to `50.0_perc` or `Length{50, Length::Unit::Perc}` in C++.
width: 50%
// Use layout dependent
width: auto
// Use widget's initial size, which is auto by default, but may be specified by individual widgets.
width: initial
```

### `resource_id`

Resource ID, may be `asset` (uses [ResourceManager](../ResourceManager.md)) or `external` (uses filesystem paths)

```qml
image: asset("gui/newFile.png")
font: external("/usr/share/fonts/TTF/SomeFont.ttf")
```

### `range`

Numeric range

```qml
range: 0..10
```

### `array`

List of elements. They may be of distinct types.

```qml
padding: [10,20,30,40]
test: [10, 20px, 5%, @TextButton { content: "test" }]
```

## Custom types

You can define custom types using `define` syntax.

```qml
// Add Splash type which inherits from ToolWindow.
define Splash: @ToolWindow

// Add Splash type which inherits from ToolWindow and define its layout.
define Splash: @ToolWindow {
    width: 300px // this will override C++ default

    main_widget: @Container {
        // ...
    }
}

// Instantiate Splash object
@Splash {
    // change properties
    width: 500px // this will override default specified in `define`
}
```

## Implementing loaders

You must:

-   inherit from `EML::EMLObject`
-   implement `EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader&)`

TODO...
