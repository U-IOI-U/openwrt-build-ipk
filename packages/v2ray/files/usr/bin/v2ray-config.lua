#!/usr/bin/lua

--[[
 Auto generate config for Project V
 Author: @libc0607
]]--

local conf_path, json_path = ...
conf_path = conf_path or "v2ray"
json_path = json_path or "/var/v2ray/config.json"


local cjson = require "luci.jsonc"
local ucursor = require "luci.model.uci".cursor()
local lip = require "luci.ip"

function v2ray_get_conf_list(op)
	local t = {}
	for k, v in pairs(ucursor:get_list(conf_path, 'v2ray', op)) do
		table.insert(t, v)
	end
	return t
end

function check_addr_type(addr)
	local ip = luci.ip.new(addr, 32)
	if ip == nil then
		return "domain"
	elseif ip:is4() then
		return "ipv4"
	elseif ip:is6() then
		return "ipv6"
	end
end

function get_ip_list_by_domain(domain)
	local domain_list = {}
	local cmd = io.popen("nslookup " .. domain .. " |grep Address | awk {'print $3'}")
	for cmd_line in cmd:lines() do
		if check_addr_type(cmd_line) == "ipv4" then
			table.insert(domain_list, cmd_line)
		elseif check_addr_type(cmd_line) == "ipv6" then
			table.insert(domain_list, cmd_line)
		end
	end
	return domain_list
end

function create_streamsetting(config)
	local setting = {
		network = config["network"],
		security = (config["tls"] == "1") and "tls" or "none",
		tcpSettings = (config["network"] == "tcp") and {
			header = {
				type = (config["htype"] ~= nil) and config["htype"] or "none",
				request = (config["htype"] == "http") and {
					version = "1.1",
					method = "GET",
					path = { (config['path'] ~= nil) and config['path'] or "/" },
					headers = {
						Host = (config['host'] ~= nil) and { config['host'], } or nil,
						User_Agent = {
							"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.75 Safari/537.36",
							"Mozilla/5.0 (iPhone; CPU iPhone OS 10_0_2 like Mac OS X) AppleWebKit/601.1 (KHTML, like Gecko) CriOS/53.0.2785.109 Mobile/14A456 Safari/601.1.46"
						},
						Accept_Encoding = {"gzip, deflate"},
						Connection = {"keep-alive"},
						Pragma = "no-cache"
					},
				} or nil,
				response = (config["htype"] == "http") and {
					version = "1.1",
					status = "200",
					reason = "OK",
					headers = {
						Content_Type = {"application/octet-stream","video/mpeg"},
						Transfer_Encoding = {"chunked"},
						Connection= {"keep-alive"},
						Pragma = "no-cache"
					},
				} or nil,
			}
		} or nil,

		kcpSettings = (config["network"] == "kcp") and {
			mtu = tonumber(config["mtu"]),
			tti = tonumber(config["tti"]),
			uplinkCapacity = tonumber(config["uplinkcapacity"]),
			downlinkCapacity = tonumber(config["downlinkcapacity"]),
			congestion = (config["congestion"] == "1") and true or false,
			readBufferSize = tonumber(config["readbufferSize"]),
			writeBufferSize = tonumber(config["writebufferSize"]),
			header = {
				type = (config["type"] ~= nil) and config["type"] or "none",
			},
		} or nil,

		wsSettings = (config["network"] == "ws") and {
			path = (config["path"] ~= nil) and config["path"] or "/",
			headers = (config["host"] ~= nil) and { Host = config["host"], } or nil,
		} or nil,

		httpSettings = (config["network"] == "h2") and {
			host = (config["host"] ~= nil) and { config["host"], } or nil,
			path = (config["path"] ~= nil) and config["path"] or "/",
		} or nil,

		dsSettings = (config["network"] == "ds") and {
			path = config["path"],
		} or nil,

		quicSettings = (config["network"] == "quic") and {
			security = (config["method"]  ~= nil) and config["method"] or "none",
			key = config["key"],
			header = {
				type = (config["type"] ~= nil) and config["type"] or "none",
			},
		} or nil,

		tlsSettings = (config["tls"] == "1") and {
			allowInsecure = (config["allowinsecure"] == "1") and true or false,
			serverName = config["servername"]
		} or nil,
	}

	return setting
end

function create_inbound(config)
	local inbound = {
		protocol = config["protocol"],
		port = tonumber(config["port"]),
		listen = config["listen"],
		tag = config["tag"],
		sniffing = (config["sniffing"] == "1") and {
			enabled = true,
			destOverride = {"http", "tls"},
		} or nil,
	}

	if (config["protocol"] == 'socks') then
		inbound["settings"] = {
			udp = (config["udp"] == '1') and true or false,
			auth = config["auth"],
			accounts = (config["auth"] == "password") and {
				{
					user = config["user"],
					pass = config["password"],
				}
			} or nil,
		}
	elseif (config["protocol"] == 'dokodemo-door') then
		inbound["settings"] = {
			network = config["network"],
			timeout = tonumber(config["timeout"]),
			followRedirect = (config["redirect"] == "1") and true or false,
			address = config["toaddress"],
			port = config["toport"],
		}
	elseif (config["protocol"] == 'http') then
		inbound["setting"] = {
			timeout = tonumber(config["timeout"]),
			allowTransparent = (config["transparent"] == "1") and true or false,
			accounts = (config["user"] and config["password"]) and {
				{
					user = config["user"],
					pass = config["password"],
				}
			} or nil,
		}
	elseif (config["protocol"] == 'vmess') then
		inbound["setting"] = {
			disableInsecureEncryption = config["disableInsecureEncryption"],
			default = {
				level = 0,
				alterId = tonumber(config["alterid"]),
			},
			clients = {
				{
					id = config["id"],
					level = (config["level"]) and config["level"] or 0,
					alterId = tonumber(config["alterid"]),
					email = "love@v2ray.com",
				},
			},
		}		
	end

	if (config["protocol"] == 'vmess') then
		inbound["streamSettings"] = create_streamsetting(config)
	end

	return inbound
end

function create_outbound(config)
	local outbound = {}

	if (config["url_type"] == "config") then
	elseif (config["url_type"] == "link") then
	else
		outbound["mux"] = (config["mux"] == "1") and { enabled = true, } or nil
		outbound["tag"] = config["tag"]
		outbound["protocol"] = config["protocol"]
		if (config["protocol"] == "socks") then
			outbound["settings"] = {
				servers = {
					{
						address = config["address"],
						port = tonumber(config["port"]),
					},
				},
			}			
		elseif (config["protocol"] == "vmess") then
			outbound["settings"] = {
				vnext = {
					{
						address = config["address"],
						port = tonumber(config["port"]),
						users = {
							{
								id = config["id"],
								alterid = tonumber(config["alterid"]),
								security = config["security"],
							},
						}
					},
				},
			}
		end

		outbound["streamSettings"] = create_streamsetting(config)
	end

	return outbound
end

function create_log(config)
	local log = {
		access = "/var/log/v2ray/access.log",
		error = "/var/log/v2ray/error.log",
		loglevel = config["level"],
	}

	return log
end

local v2ray	= {
	inbounds = {},
	outbounds = {},
	-- api = {
	-- 	tag = "api",
	-- 	services = {
	-- 		"HandlerService",
	-- 		"LoggerService",
	-- 		"StatsService",
	-- 	},
	-- },
}

-- create inbounds
ucursor:foreach(conf_path, 'in', function(sec)
	if (sec["enable"] == '1') then
		table.insert(v2ray['inbounds'], create_inbound(sec))
	end
	return true
end)

-- create outbounds
ucursor:foreach(conf_path, 'out', function(sec)
	if (sec["enable"] == '1') then
		table.insert(v2ray['outbounds'], create_outbound(sec))
	end
	return true
end)

-- create log
ucursor:foreach(conf_path, 'log', function(sec)
	v2ray["log"] = create_log(sec)
	return true
end)

-- Generate config json to <json_path>
local json_raw = cjson.stringify(v2ray)
local json_file = io.open(json_path, "w+")
io.output(json_file)
io.write(json_raw)
io.close(json_file)

-- change '_' to '-'
local keys_including_minus = {"User_Agent", "Content_Type", "Accept_Encoding", "Transfer_Encoding"}
local keys_corrected = {"User-Agent", "Content-Type", "Accept-Encoding", "Transfer-Encoding"}
for k, v in pairs(keys_including_minus) do
	os.execute("sed -i 's/" ..v.. "/" ..keys_corrected[k].. "/g' " .. json_path)
end

-- change "\/" to "/"
os.execute("sed -i 's/\\\\\\//\\//g' ".. json_path)

print("V2ray config generated at " .. json_path)


