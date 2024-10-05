defmodule BACNet.Gateway.Object do
  @moduledoc """
  Add and manage BACnet objects on Routed Devices.
  """

  @doc """
  Creates a new analog input object.

  ## Parameters

    - `pid`: The PID of the GenServer managing the BACnet communication.
    - `device_id`: The ID of the BACnet device where the object will be created.
    - `object_id`: The unique ID for the new analog input object.
    - `name`: A unique name for the object.
    - `unit`: The unit of measurement for the analog input, represented as an atom.
  """
  @spec create_analog_input(
          pid       :: pid,
          device_id :: integer,
          object_id :: integer,
          name      :: String.t,
          unit      :: atom
        ) :: :ok | {:error, term}
  def create_analog_input(pid, device_id, object_id, name, unit) do
    request = {
      :create_routed_analog_input,
      device_id,
      object_id,
      name,
      unit,
    }

    GenServer.call(pid, request)
  end
end
