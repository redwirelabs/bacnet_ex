defmodule Helper do
  def udp_start!() do
    {:ok, pid} = BACNetUDP.start_link([])
    pid
  end

  def kill, do: BACNetUDP.kill()

  def create_gateway(pid, gateway_instance \\ 260001) do
    BACNet.Gateway.create(pid, gateway_instance, "Relay", "Gateway", "Array Relay", "v1.0.0")
  end

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

alias Helper, as: H
