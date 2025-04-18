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
  Initialize a Gateway device with a virtual device attached to its
  network.

  Use it as:

  ```
  pid = initialize_gateway_with_a_vdev()
  ```

  Or providing the `gateway_instance` and `vdev_instance` identifiers as
  parameters.

  It is important to store the `pid` to then, for example, create more
  virtual devices.

  This function performs the next steps:

    1. Start `BACNetUDP` GenServer.
    2. Create a Gateway device.
    3. Create a virtual device with an analog-input object.
  """
  @spec initialize_gateway_with_a_vdev(integer(), integer()) :: pid()
  def initialize_gateway_with_a_vdev(gateway_instance \\ 1000, vdev_instance \\ 1001) do
    {:ok, pid} = BACNetUDP.start_link([])
    :ok = create_gateway(pid, gateway_instance)
    :ok = create_vdev(pid, vdev_instance)
    pid
  end

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
  def create_gateway(pid, gateway_instance \\ 1000) do
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
        :rand.uniform(100),
        object.name,
        object.description,
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
end
