defmodule BACNet do
  @moduledoc """
  BACNet client.
  """

  require Logger

  defmodule Device do
    @typedoc "Placeholder"
    @type t :: term
  end

  @spec add_device(device :: Device.t) :: :ok | {:error, term}
  def add_device(device) do
    GenServer.call({:global, :bacnetd}, {:add_device, device})
  end

  @doc false
  defdelegate ei_log(level, term), to: Logger, as: :log
end
