defmodule :ncnn_nif do
  @moduledoc false
  @on_load :load_nif
  def load_nif do
    require Logger
    nif_file = '#{:code.priv_dir(:elixir_ncnn)}/elixir_ncnn'

    case :erlang.load_nif(nif_file, 0) do
      :ok -> :ok
      {:error, {:reload, _}} -> :ok
      {:error, reason} -> Logger.warn("Failed to load nif: #{inspect(reason)}")
    end
  end

  def net_new_net, do: :erlang.nif_error(:not_loaded)
  def net_load_param(_opts \\ []), do: :erlang.nif_error(:not_loaded)
  def net_load_model(_opts \\ []), do: :erlang.nif_error(:not_loaded)
  def net_forward(_opts \\ []), do: :erlang.nif_error(:not_loaded)
end
