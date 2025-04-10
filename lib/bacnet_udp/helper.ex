defmodule BACNetUDP.Helper do
  @moduledoc """
  This `Helper` module is mainly for a more comfortable debugging
  of the `bacnetd` using UDP sockets as an alternative to the
  Elixir's `Port`.

  All the functions of this helper module are short aliases for
  the ones available in `BACNetUDP` and `BACNet.Gateway`.

  It can also be used as `H`.
  """

  @doc """
  Start BACNetUDP GenServer.

  It returns the `pid` of the GenServer. Store it in a variable
  to reuse with the `create_*` functions.
  """
  @spec udp_start!() :: pid()
  def udp_start!() do
    {:ok, pid} = BACNetUDP.start_link([])
    pid
  end

  @doc """
  Kill the BACNetUDP GenServer.

  Use it to start again the GenServer without closing IEx and
  starting again. In that way you can recover the functions
  executed previously.
  """
  @spec kill() :: :ok
  def kill, do: BACNetUDP.kill()

  @doc """
  Creates the gateway device.

  If not `gateway_instance` provided, the default value is 260001.
  """
  @spec create_gateway(pid(), integer()) :: :ok | {:error, any()}
  def create_gateway(pid, gateway_instance \\ 260001) do
    BACNet.Gateway.create(pid, gateway_instance, "Relay", "Gateway", "Array Relay", "v1.0.0")
  end

  @doc """
  Creates a virtual device with an analog-input object.

  If not `vdev_instance` provided, the default value is 1001.

  The virtual device object will be always the same, in case it is necessary
  to provide different name, description, model and version, ask Iván for
  a helper function or use `BACNet.Gateway.create_routed_device/6` (type
  `h BACNet.Gateway.create_routed_device` to get documentation about each
  of the 6 required parameters).
  """
  @spec create_vdev(pid(), integer()) :: :ok
  def create_vdev(pid, vdev_instance \\ 1001) do
    vdev =
      %{
        id: vdev_instance,
        name: "AP1",
        description: "Air Purifier Ceiling Hung",
        model: "Array AC2",
        version: "1.0.0",
        objects: nil
      }

    :ok = BACNet.Gateway.create_routed_device(
      pid,
      vdev.id,
      vdev.name,
      vdev.description,
      vdev.model,
      vdev.version)

    object = %{
        name: "Air Quality",
        type: :analog_input,
        description: "RESET Air Index quality rating",
        units: :percent
      }

    :ok =
      BACNet.Gateway.Object.create_analog_input(
        pid,
        vdev.id,
        1,
        object.name,
        object.units
      )
  end

  @doc """
  Set the present-value of the given analog-input.
  """
  @spec set_analog_input_object(pid(), integer(), integer(), float()) :: :ok | {:error, any()}
  def set_analog_input_object(pid, vdev_id, object_id, value) do
    BACNet.Gateway.Object.set_analog_input_present_value(
      pid,
      vdev_id,
      object_id,
      value
    )
  end

  # ivan2ed - It is here as a reference.
  # todo 0 - When the reference won't be more necessary, remove it.
  # {~c"BACNET_IFACE", args[:network_interface]},
  # {~c"BACNET_IP_PORT", args[:network_port]},
  # {~c"BACNET_NETWORK_ID", args[:network_id]},
  # {~c"BACNET_VENDOR_ID", args[:vendor_id]},
  # {~c"BACNET_VENDOR_NAME", args[:vendor_name]},
  # @network_interface "wlp0s20f3"
  # @network_port "47818"
  # @vendor_id "1532"
  # def start!() do
  #     {:ok, pid} = BACNet.start_link(
  #     network_port: @network_port,
  #     network_interface: @network_interface,
  #     vendor_id: @vendor_id,
  #     vendor_name: "Fellowes Inc.",
  #     network_id: @vendor_id
  #   )
  #   pid
  # end
end
