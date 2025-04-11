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
    pid         :: pid,
    device_id   :: integer,
    object_id   :: integer,
    name        :: String.t,
    description :: String.t,
    unit        :: atom
  ) :: :ok | {:error, term}
  def create_analog_input(pid, device_id, object_id, name, description, unit) do
    GenServer.call(pid, {
      :create_routed_analog_input,
      device_id,
      object_id,
      name,
      description,
      unit,
    })
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
    GenServer.call(pid, {
      :set_routed_analog_input_value,
      device_id,
      object_id,
      value,
    })
  end

  @doc """
  Creates a new multistate input object.

  ## Parameters

    - `pid`: The PID of the GenServer managing the BACnet communication.
    - `device_id`: The ID of the BACnet device where the object will be created.
    - `object_id`: The unique ID for the new multistate input object.
    - `name`: A unique name for the object.
    - `states`: A set of strings representing the object's states.
  """
  @spec create_multistate_input(
    pid         :: pid,
    device_id   :: integer,
    object_id   :: integer,
    name        :: String.t,
    description :: String.t,
    states      :: [String.t]
  ) :: :ok | {:error, term}
  def create_multistate_input(pid, device_id, object_id, name, description, states) do
    GenServer.call(pid, {
      :create_routed_multistate_input,
      device_id,
      object_id,
      name,
      description,
      states,
    })
  end

  @doc """
  Sets the present value of a multistate input object.

  ## Parameters

  - `pid`: The PID of the GenServer managing the BACnet communication.
  - `device_id`: The ID of the BACnet device containing the object.
  - `object_id`: The unique ID of the multistate input object to update.
  - `value`: The new present value to be set for the multistate input.
  """
  @spec set_multistate_input_present_value(
    pid :: pid,
    device_id :: integer,
    object_id :: integer,
    value :: non_neg_integer
  ) :: :ok | {:error, term}
  def set_multistate_input_present_value(pid, device_id, object_id, value) do
    GenServer.call(pid, {
      :set_routed_multistate_input_value,
      device_id,
      object_id,
      value,
    })
  end

  @doc """
  Creates a new command object.

  ## Parameters

    - `pid`: The PID of the GenServer managing BACnet communications.
    - `device_id`: The ID of the BACnet device where the object will be created.
    - `object_id`: The unique ID for the new command object.
    - `name`: A unique name for the object.
    - `description`: A short description for the command object.
  """
  @spec create_command(
    pid         :: pid,
    device_id   :: integer,
    object_id   :: integer,
    name        :: String.t,
    description :: String.t,
    value       :: nil | non_neg_integer
  ) :: :ok | {:error, term}
  def create_command(pid, device_id, object_id, name, description, value \\ nil) do
    GenServer.call(pid, {
      :create_routed_command,
      device_id,
      object_id,
      name,
      description,
      value
    })
  end

  @doc """
  Set the status of an active command.

  ## Parameters

    - `pid`: The PID of the GenServer managing BACnet communications.
    - `device_id`: The ID of the BACnet device where the object will be created.
    - `object_id`: The unique ID for the new command object.
    - `status`: The status of the command.
  """
  @spec set_command_status(
    pid       :: pid,
    device_id :: integer,
    object_id :: integer,
    status     :: :succeeded | :failed
  ) :: :ok | {:error, term}
  def set_command_status(pid, device_id, object_id, status) do
    GenServer.call(pid, {
      :set_routed_command_status,
      device_id,
      object_id,
      status
    })
  end

  def create_characterstring_value(pid, device_id, object_id, name, description, value) do
    GenServer.call(pid, {
      :create_characterstring_value,
      device_id,
      object_id,
      name,
      description,
      value
    })
  end

  def create_binary_input(pid, device_id, object_id, name, description, active_text, inactive_text, polarity, value) do
    GenServer.call(pid, {
      :create_binary_input,
      device_id,
      object_id,
      name,
      description,
      active_text,
      inactive_text,
      polarity,
      value
    })
  end

  def set_binary_input_present_value(pid, device_id, object_id, value) do
    GenServer.call(pid, {
      :set_binary_input_value,
      device_id,
      object_id,
      value
    })
  end
end
