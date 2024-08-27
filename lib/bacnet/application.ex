defmodule Bacnet.Application do
  @moduledoc false

  use Application

  @impl true
  def start(_type, _args) do
    children = children()
    opts = [strategy: :one_for_one, name: Bacnet.Supervisor]

    Supervisor.start_link(children, opts)
  end

  defp children(), do: []
end
