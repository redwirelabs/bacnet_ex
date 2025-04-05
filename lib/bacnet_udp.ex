defmodule BACNetUDP do
  use GenServer

  require Logger

  @port1 8080                  # Local port used for receiving UDP messages.
  @port2 8081                  # Remote port where the other server/client is listening.
  @default_remote_ip {127, 0, 0, 1}
  @default_message "Hello from Program 1"

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
  """
  def send_message(message \\ @default_message, remote_ip \\ @default_remote_ip, remote_port \\ @port2) do
    GenServer.cast(__MODULE__, {:send_message, message, remote_ip, remote_port})
  end

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
            # Send an initial message.
            :ok = :gen_udp.send(send_socket, @default_remote_ip, @port2, @default_message)
            IO.puts("Sent initial message to the other server: #{@default_message}")

            # Save both sockets in the state.
            {:ok, %{recv_socket: recv_socket, send_socket: send_socket}}

          {:error, reason} ->
            IO.puts("Failed to open sending socket: #{inspect(reason)}")
            {:stop, reason}
        end

      {:error, reason} ->
        IO.puts("Failed to open receiving socket on port #{@port1}: #{inspect(reason)}")
        {:stop, reason}
    end
  end

  @impl GenServer
  def handle_cast({:send_message, message, remote_ip, remote_port}, state) do
    case :gen_udp.send(state.send_socket, remote_ip, remote_port, message) do
      :ok ->
        IO.puts("Cast: sent message to #{:inet.ntoa(remote_ip)}:#{remote_port} -> #{message}")
        {:noreply, state}
      {:error, reason} ->
        IO.puts("Cast: failed to send message: #{inspect(reason)}")
        {:noreply, state}
    end
  end

  @impl GenServer
  def handle_call(cmd, from, state) do
    encoded_term = :erlang.term_to_binary({:"$gen_call", from, cmd})
    case :gen_udp.send(state.send_socket, @default_remote_ip, @port2, encoded_term) do
      :ok ->
        IO.puts("Sync: Sent message to #{:inet.ntoa(@default_remote_ip)}:#{@port2}")
        # You might want to wait for a reply or simply return :ok.
        {:reply, :ok, state}

      {:error, reason} ->
        IO.puts("Sync: Failed to send message: #{inspect(reason)}")
        {:reply, {:error, reason}, state}
    end
  end

  @impl GenServer
  def handle_info(message, state) do
    IO.puts("Received unhandled message: #{inspect(message)}")

    try do
      message
      |> :erlang.binary_to_term
      |> process_message
      |> IO.inspect(label: "Received unhandled message (processed)")
    catch
      _ -> nil
    end
    {:noreply, state}
  end

  defp receive_loop(socket) do
    case :gen_udp.recv(socket, 1000, 5000) do
      {:ok, {ip, port, data}} ->
        IO.puts("Received from #{:inet.ntoa(ip)}:#{port} -> #{data}")
        # ivan - Enable this trick when testing with the C socket server
        # if data is processed.
        # data = :erlang.term_to_binary(data)
        try do
          data
          |> :erlang.binary_to_term
          |> process_message
        catch
          _ -> nil
        end

        receive_loop(socket)

      {:error, :timeout} ->
        receive_loop(socket)

      {:error, reason} ->
        IO.puts("Error receiving UDP data: #{inspect(reason)}")
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
