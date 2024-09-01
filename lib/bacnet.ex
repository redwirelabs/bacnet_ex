defmodule Bacnet do
  @moduledoc """
  BACNet client.
  """

  require Logger

  @spec add_device(device :: term) :: :ok | {:error, term}
  def add_device(device) do
    GenServer.call({:global, :bacnetd}, {:add_device, device})
  end

  @doc false
  defdelegate ei_log(level, term), to: Logger, as: :log
end
