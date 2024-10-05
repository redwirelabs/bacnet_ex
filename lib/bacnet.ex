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
  def init(args) do
    bacnetd_exe = "#{:code.priv_dir(:bacnet)}/bacnetd"

    env =
      []
      |> maybe_add_env(~c"BACNET_IFACE", args[:network_interface])
      |> maybe_add_env(~c"BACNET_NETWORK_ID", args[:network_id])
      |> maybe_add_env(~c"BACNET_VENDOR_ID", args[:vendor_id])
      |> maybe_add_env(~c"BACNET_VENDOR_NAME", args[:vendor_name])

    port =
      Port.open(
        {:spawn, bacnetd_exe},
        [:binary, :use_stdio, packet: 4, env: env]
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

  defp maybe_add_env(env, _key, nil), do: env
  defp maybe_add_env(env, key, value), do: [{key, to_charlist(value)} | env]
end
