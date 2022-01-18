# ElixirNcnn

Only tested on Linux (Ubuntu 20.04 x86_64).

Not maintained. ncnn is unstable and often crashes on arm64 host. Not sure about the reason.

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

## Supported Models
- MobileNetSSD
- MobileNetSSDLite V2
- MobileNetSSDLite V3

Test code,
```elixir
# you can load an image by whatever way you prefer
# but `image_data` has to be bgr888 format at the moment
# below shows how to do this with evision
## read an image from file
{:ok, mat} = OpenCV.imread(image_path)
## get shapes
{:ok, {rows, cols, _}} = OpenCV.Mat.shape(mat)
## convert to binary data
{:ok, image_data} = OpenCV.Mat.to_binary(mat)

# load MobileNetSSDLite V3
{:ok, mobilenetv3} = Ncnn.Models.MobileNet.load(:v3)
# predict
{:ok, pred} = Ncnn.Models.MobileNet.predict(mobilenetv3, image_data, cols, rows) ; 0

## this function uses evision at the moment
Ncnn.Models.MobileNet.translate_pred(mat, pred)
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

