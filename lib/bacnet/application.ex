defmodule BACNet.Application do
  @moduledoc false

  use Application

  @impl true
  def start(_type, _args) do
    bacnetd_exe = "#{:code.priv_dir(:bacnet)}/bacnetd"
    bacnetd_args = [
      "--cookie", Node.get_cookie |> to_string,
      "--nodename", Node.self |> to_string,
    ]

    children = [
      {MuonTrap.Daemon, [bacnetd_exe, bacnetd_args, []]}
    ]

    Supervisor.start_link(
      children,
      strategy: :one_for_one,
      name: BACNet.Supervisor
    )
  end
end
