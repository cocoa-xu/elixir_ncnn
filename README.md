# ElixirNcnn

Only tested on Linux (Ubuntu 20.04 x86_64).

## Current Status
To test it, create a new `mix` library/applcation, and add `:evision` and `:elixir_ncnn` to deps.

```elixir
defp deps do
  [
      {:evision, github: "cocoa-xu/evision"},
      {:elixir_ncnn, github: "cocoa-xu/elixir_ncnn"}
  ]
end
```

Then download `.param` file and model file `.bin` from [here](https://github.com/nihui/ncnn-assets/tree/master/models).

- [mobilenetv3_ssdlite_voc.bin](https://github.com/nihui/ncnn-assets/blob/master/models/mobilenetv3_ssdlite_voc.bin)
- [mobilenetv3_ssdlite_voc.param](https://github.com/nihui/ncnn-assets/blob/master/models/mobilenetv3_ssdlite_voc.param)

Test code,
```elixir
{:ok, mobilenetv3} = Ncnn.MobileNetV3SsdLite.load(
  "/path/to/mobilenetv3_ssdlite_voc.param", 
  "/path/to/mobilenetv3_ssdlite_voc.bin"
)
{:ok, mat, pred} = Ncnn.MobileNetV3SsdLite.predict(mobilenetv3, "/path/to/img.jpg") ; 0
Ncnn.MobileNetV3SsdLite.translate_pred(mat, pred)
```

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `elixir_ncnn` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:elixir_ncnn, "~> 0.1.0"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/elixir_ncnn](https://hexdocs.pm/elixir_ncnn).

