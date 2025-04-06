defmodule BACNetUDP do
  use GenServer

  require Logger

  # Local port used for receiving UDP messages.
  @port1 8080
  # Remote port where the other server/client is listening.
  @port2 8081
  @default_remote_ip {127, 0, 0, 1}
  @default_message "Hello from Elixir side (test message)"

  ## Client API

  @doc """
  Start the BACnet client.
  """
  @spec start_link(any, GenServer.options()) :: GenServer.on_start()
  def start_link(args, opts \\ []) do
    GenServer.start_link(__MODULE__, args, opts ++ [name: __MODULE__])
  end

  @doc """
  Asynchronously send a UDP message using GenServer.cast.
  You can override the default remote IP, port, and message.

  todo 1 - This function is not intended to be used in the module, however
  by the moment can help to debug. When it isn't more necessary
  can be removed safely.
  """
  def send_message(
        message \\ @default_message,
        remote_ip \\ @default_remote_ip,
        remote_port \\ @port2
      ) do
    GenServer.cast(__MODULE__, {:send_message, message, remote_ip, remote_port})
  end

  @doc """
  Helper function to kill the current GenServer.
  """
  def kill(), do: GenServer.cast(__MODULE__, {:exit, :shutdown})

  @impl GenServer
  def init(_opts) do
    # Open a UDP socket for receiving messages on @port1.
    case :gen_udp.open(@port1, [:binary, active: false, reuseaddr: true]) do
      {:ok, recv_socket} ->
        # Start a Task to continuously receive UDP messages.
        Task.start(fn -> receive_loop(recv_socket) end)

        # Open a UDP socket for sending messages (using port 0 lets the OS choose an ephemeral port).
        case :gen_udp.open(0, [:binary]) do
          {:ok, send_socket} ->
            {:ok, %{recv_socket: recv_socket, send_socket: send_socket}}

          {:error, reason} ->
            Logger.error("Failed to open sending socket: #{inspect(reason)}")
            {:stop, reason}
        end

      {:error, reason} ->
        Logger.error("Failed to open receiving socket on port #{@port1}: #{inspect(reason)}")
        {:stop, reason}
    end
  end

  @impl GenServer
  def handle_cast({:send_message, message, remote_ip, remote_port}, state) do
    case :gen_udp.send(state.send_socket, remote_ip, remote_port, message) do
      :ok ->
        Logger.debug("Cast: sent message to #{:inet.ntoa(remote_ip)}:#{remote_port} -> #{message}")
        {:noreply, state}

      {:error, reason} ->
        Logger.debug("Cast: failed to send message: #{inspect(reason)}")
        {:noreply, state}
    end
  end

  @impl GenServer
  def handle_cast({:exit, reason}, _state) do
    Logger.info("Exit received with reason #{reason}")
    exit(reason)
  end

  @impl GenServer
  def handle_call(cmd, from, state) do
    encoded_term = :erlang.term_to_binary({:"$gen_call", from, cmd})

    case :gen_udp.send(state.send_socket, @default_remote_ip, @port2, encoded_term) do
      :ok ->
        # Logger.debug("Sync: Sent message to #{:inet.ntoa(@default_remote_ip)}:#{@port2}")
        {:reply, :ok, state}

      {:error, reason} ->
        Logger.warning("Sync: Failed to send message: #{inspect(reason)}")
        {:reply, {:error, reason}, state}
    end
  end

  @impl GenServer
  def handle_info(message, state) do
    Logger.warning("Received unhandled message: #{inspect(message)}")

    try do
      message
      |> :erlang.binary_to_term()
      |> process_message
    catch
      _ -> nil
    end

    {:noreply, state}
  end

  @impl GenServer
  def terminate(reason, _state) do
    Logger.info("#{__MODULE__} has been shutdown #{inspect {:reason, reason}}")
  end

  defp receive_loop(socket) do
    # ivan - for testing, in infinity
    case :gen_udp.recv(socket, 1500, :infinity) do
      {:ok, {_ip, _port, data}} ->
        # todo 0 - Remove when it isn't necessary to debug it anymore.
        # data
        # |> :erlang.binary_to_term()
        # |> IO.inspect(label: "term")

        try do
          data
          |> :erlang.binary_to_term()
          |> process_message
        catch
          _ -> nil
        end

        receive_loop(socket)

      {:error, :timeout} ->
        receive_loop(socket)

      {:error, :closed} ->
        nil

      {:error, reason} ->
        Logger.error("Error receiving UDP data: #{inspect(reason)}")
        receive_loop(socket)
    end
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
