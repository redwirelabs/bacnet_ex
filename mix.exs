defmodule BACNet.MixProject do
  use Mix.Project

  def project do
    [
      app: :bacnet,
      version: "0.1.0",
      elixir: "~> 1.16",
      aliases: aliases(),
      description: description(),
      package: package(),
      deps: deps(),
      docs: docs(),
      compilers: [:cmake] ++ Mix.compilers(),
      cmake_env: %{"MAKEFLAGS" => "-j#{nproc()}"},
      start_permanent: Mix.env() == :prod,
      test_coverage: [tool: ExCoveralls, test_task: "espec"],
      dialyzer: [
        list_unused_filters: true,
        plt_file: {:no_warn, plt_file_path()},
      ],
      preferred_cli_env: [
        coveralls: :test,
        "coveralls.detail": :test,
        "coveralls.post": :test,
        "coveralls.html": :test,
        "coveralls.show": :test,
        espec: :test,
      ],
    ]
  end

  def application do
    [
      extra_applications: [:logger],
    ]
  end

  defp aliases do
    [
      "coveralls.show": ["coveralls.html", &open("cover/excoveralls.html", &1)],
      "docs.show": ["docs", &open("doc/index.html", &1)],
      test: "coveralls",
    ]
  end

  defp deps do
    [
      {:dialyxir, "~> 1.4", only: :dev, runtime: false},
      {:espec, "~> 1.9", only: :test},
      {:elixir_cmake, "~> 0.8.0"},
      {:excoveralls, "~> 0.18", only: :test},
      {:ex_doc, "~> 0.34", only: :dev, runtime: false},
      {:muontrap, "~> 1.5"},
    ]
  end

  defp description do
    """
    BACNet elixir library
    """
  end

  defp docs do
    [
      main: "readme",
      extras: ["README.md", "LICENSE"],
    ]
  end

  defp package do
    [
      licenses: ["MIT"],
      links: %{"GitHub" => "https://github.com/redwirelabs/bacnet_ex"},
      maintainers: ["Abelino Romo"],
      files: [
        ".tool-versions",
        "CMakeLists.txt",
        "LICENSE",
        "README.md",
        "lib",
        "mix.exs",
        "src",
      ],
    ]
  end

  # Open a file with the default application for its type.
  defp open(file, _args) do
    open_command =
      System.find_executable("xdg-open") # Linux
      || System.find_executable("open")  # Mac
      || raise "Could not find executable 'open' or 'xdg-open'"

    System.cmd(open_command, [file])
  end

  # Path to the dialyzer .plt file.
  defp plt_file_path do
    [Mix.Project.build_path(), "plt", "dialyxir.plt"]
    |> Path.join()
    |> Path.expand()
  end

  defp nproc do
    :erlang.system_info(:logical_processors)
  end
end
