defmodule Bacnet.Application do
  @moduledoc false

  use Application

  @impl true
  def start(_type, _args) do
    children = children()
    opts = [strategy: :one_for_one, name: Bacnet.Supervisor]

    Supervisor.start_link(children, opts)
  end

  defp children() do
    [
      {Bacnet.Logger, nil},
      {Bacnet.Heartbeat, nil},
      {MuonTrap.Daemon, [bacnetd_exec(), bacnetd_args(), []]}
    ]
  end

  defp bacnetd_exec do
    "#{:code.priv_dir(:bacnet)}/bacnetd"
  end

  defp bacnetd_args do
    [
      "--cookie", Node.get_cookie() |> to_string(),
      "--nodename", Node.self() |> to_string(),
    ]
  end
end
