defmodule Bacnet.Heartbeat do
  @moduledoc """
  Heartbeat server accepting periodic messages, keeping the cnode alive.
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

  @impl GenServer
  def handle_info(:heartbeat, state) do
    Logger.debug("Heartbeat received from bacnetd")

    {:noreply, state}
  end
end
