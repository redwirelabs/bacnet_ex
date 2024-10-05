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
end
