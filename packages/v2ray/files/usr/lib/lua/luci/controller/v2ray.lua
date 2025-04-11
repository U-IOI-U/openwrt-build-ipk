module("luci.controller.v2ray", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/v2ray") then
		return
	end
	entry({"admin", "services", "v2ray"}, cbi("v2ray"), _("V2ray"), 99).dependent = true
	entry({"admin","services","v2ray","status"},call("act_status")).leaf=true
end

function act_status()
  local e={}
  e.running=nixio.fs.access("/var/run/v2ray.pid")
  luci.http.prepare_content("application/json")
  luci.http.write_json(e)
end
