defmodule BACNet do
  @moduledoc """
  BACNet client.
  """

  use GenServer

  require Logger

  defmodule State do
    @doc false

    defstruct [
      :owner,
      :port,
    ]
  end

  @doc """
  Start the BACnet client.
  """
  @spec start_link(any, GenServer.options()) :: GenServer.on_start()
  def start_link(args, opts \\ []) do
    args = Keyword.put_new(args, :owner, self())

    GenServer.start_link(__MODULE__, args, opts)
  end

  @impl GenServer
  def init(args) do
    bacnetd_exe = Path.join(:code.priv_dir(:bacnet), "bacnetd")

    env =
      [
        {~c"BACNET_IFACE", args[:network_interface]},
        {~c"BACNET_NETWORK_ID", args[:network_id]},
        {~c"BACNET_VENDOR_ID", args[:vendor_id]},
        {~c"BACNET_VENDOR_NAME", args[:vendor_name]},
      ]
      |> Enum.reject(fn {_key, value} -> is_nil(value) end)
      |> Enum.map(fn {key, value} -> {key, to_charlist(value)} end)

    port =
      Port.open(
        {:spawn, bacnetd_exe},
        [:binary, :use_stdio, packet: 4, env: env]
      )

    state = %State{
      owner: args[:owner],
      port: port,
    }

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
    maybe_message =
      try do
        :erlang.binary_to_term(data)
      rescue
        _ -> {:error, :invalid_term, data}
      end

    case maybe_message do
      {:log, level, message} -> Logger.log(level, message)
      {:"$gen_reply", to, result} -> GenServer.reply(to, result)
      {:"$event", message} -> send(state.owner, message)
      {:error, :invalid_term, data} -> Logger.warning("Received bad data #{inspect(data)}")
    end

    {:noreply, state}
  end
end
