defmodule BACNet.Gateway do
  @moduledoc """
  Create and manage a Gateway and its Routed devices.
  """

  @doc """
  Creates a Gateway device. Only a single Gateway device is allowed per
  BACnet process and it is assumed to be the first Routed device created.
  """
  @spec create(
          pid :: pid,
          gateway_id :: integer,
          name :: String.t(),
          description :: String.t(),
          model :: String.t(),
          firmware_version :: String.t()
        ) :: :ok | {:error, term}
  def create(pid, gateway_id, name, description, model, firmware_version) do
    request = {
      :create_gateway,
      gateway_id,
      name,
      description,
      model,
      firmware_version,
    }

    GenServer.call(pid, request)
  end

  @doc """
  Creates a new routed device to the BACnet gateway.

  ## Parameters

  - `pid`: The PID of the GenServer managing the BACnet communication.
  - `device_id`: The unique instance number for the new routed device.
  - `name`: A human-readable name for the device.
  - `description`: A brief description of the device.
  - `model`: The model name or number of the device.
  - `firmware_version`: The firmware version currently running on the device.
  """
  @spec create_routed_device(
          pid :: pid,
          device_id :: integer,
          name :: String.t(),
          description :: String.t(),
          model :: String.t(),
          firmware_version :: String.t()
        ) :: :ok | {:error, term}
  def create_routed_device(pid, device_id, name, description, model, firmware_version) do
    request = {
      :create_routed_device,
      device_id,
      name,
      description,
      model,
      firmware_version,
    }

    GenServer.call(pid, request)
  end
end
