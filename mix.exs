defmodule ElixirNcnn.MixProject do
  use Mix.Project

  @app :elixir_ncnn
  @version "0.1.0-dev"
  @ncnn_version "20211208"

  def project do
    [
      app: :elixir_ncnn,
      version: "0.1.0",
      elixir: "~> 1.11-dev",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      elixirc_paths: elixirc_paths(Mix.env()),
      deps: deps(),
      make_env: %{
        "NCNN_VERSION" => @ncnn_version,
        "MAKE_BUILD_FLAGS" => System.get_env("MAKE_BUILD_FLAGS", "-j#{System.schedulers_online()}"),
        "USE_NCNN_TOOLCHAIN" => System.get_env("USE_NCNN_TOOLCHAIN", "YES")
      },
      xref: [
        exclude: [
          OpenCV,
          OpenCV.Mat
        ]
      ]
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp elixirc_paths(_), do: ~w(lib)

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:elixir_make, "~> 0.6"},
      {:ex_doc, "~> 0.23", only: :dev, runtime: false}
    ]
  end
end
