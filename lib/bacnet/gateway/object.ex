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

  @doc """
  Sets the present value of an analog input object.

  ## Parameters

    - `pid`: The PID of the GenServer managing the BACnet communication.
    - `device_id`: The ID of the BACnet device containing the object.
    - `object_id`: The unique ID of the analog input object to update.
    - `value`: The new present value to be set for the analog input.
  """
  @spec set_analog_input_present_value(
          pid :: pid,
          device_id :: integer,
          object_id :: integer,
          value :: float
        ) :: :ok | {:error, term}
  def set_analog_input_present_value(pid, device_id, object_id, value) do
    request =
      {:set_routed_analog_input_value, device_id, object_id, value}

    GenServer.call(pid, request)
  end

  @doc """
  Creates a new multistate input object.

  ## Parameters

    - `pid`: The PID of the GenServer managing the BACnet communication.
    - `device_id`: The ID of the BACnet device where the object will be created.
    - `object_id`: The unique ID for the new multistate input object.
    - `name`: A unique name for the object.
  """
  @spec create_multistate_input(
          pid       :: pid,
          device_id :: integer,
          object_id :: integer,
          name      :: String.t,
          states    :: [String.t]
        ) :: :ok | {:error, term}
  def create_multistate_input(pid, device_id, object_id, name, states) do
    request =
      {:create_routed_multistate_input, device_id, object_id, name, states}

    GenServer.call(pid, request)
  end
end
