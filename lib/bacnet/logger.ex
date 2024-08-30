defmodule Bacnet.Logger do
  @moduledoc """
  Log server for the cnode.
  """

  use GenServer

  require Logger

  @levels [
    :emergency,
    :alert,
    :critical,
    :error,
    :warning,
    :notice,
    :info,
    :debug,
  ]

  @spec start_link(any) :: GenServer.on_start()
  def start_link(_opts) do
    GenServer.start_link(__MODULE__, nil, name: __MODULE__)
  end

  @impl GenServer
  def init(_) do
    {:ok, nil}
  end

  @impl GenServer
  def handle_info({level, term}, state) when level in @levels do
    Logger.log(level, maybe_inspect(term))

    {:noreply, state}
  end

  defp maybe_inspect(term) when is_binary(term), do: term
  defp maybe_inspect(term), do: inspect(term)
end
