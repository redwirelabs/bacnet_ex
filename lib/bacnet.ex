defmodule BACNet do
  @moduledoc """
  BACNet client.
  """

  use GenServer

  require Logger

  defmodule State do
    @doc false

    defstruct [:port]
  end

  @doc """
  Start the BACnet client.
  """
  @spec start_link(any, GenServer.options()) :: GenServer.on_start()
  def start_link(args, opts \\ []) do
    GenServer.start_link(__MODULE__, args, opts)
  end

  @impl GenServer
  def init(_args) do
    bacnetd_exe = "#{:code.priv_dir(:bacnet)}/bacnetd"

    port = Port.open(
      {:spawn, bacnetd_exe},
      [
        :binary,
        :use_stdio,
        packet: 4,
        env: [],
      ]
    )

    state = %State{port: port}

    {:ok, state}
  end

  @impl GenServer
  def handle_call(cmd, from, state) do
    encoded_term = :erlang.term_to_binary({:"$gen_call", from, cmd})
    Port.command(state.port, encoded_term)

    {:noreply, state}
  end

  @impl GenServer
  def handle_info({_port, {:data, data}}, state) do
    try do
      data
      |> :erlang.binary_to_term
      |> process_message
    catch
      _ -> nil
    end

    {:noreply, state}
  end

  defp process_message({:log, level, message}) do
    Logger.log(level, message)
  end

  defp process_message({:"$gen_reply", to, result}) do
    GenServer.reply(to, result)
  end

  defp process_message(unknown) do
    Logger.warning("Unknown message received #{inspect(unknown)}")
  end
end
