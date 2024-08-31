defmodule Bacnet do
  @moduledoc """
  BACNet client.
  """

  use GenServer

  require Logger

  @spec start_link(any) :: GenServer.on_start()
  def start_link(_opts) do
    GenServer.start_link(__MODULE__, nil, name: __MODULE__)
  end

  @impl GenServer
  def init(_) do
    {:ok, nil}
  end

  @doc false
  defdelegate ei_log(level, term), to: Logger, as: :log
end
