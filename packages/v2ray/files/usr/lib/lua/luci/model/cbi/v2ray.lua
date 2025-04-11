

m = Map("v2ray")
m.title	= translate("v2ray config")
m.description = translate(" A unified platform for anti-censorship.")

m:section(SimpleSection).template  = "v2ray/v2ray_status"

s = m:section(TypedSection, "in", translate("V2ray Inbound"))
s.addremove = true
s.anonymous = false

a = s:option(Flag, "enable", translate("Enabled"))
a.rmempty = false

a = s:option(ListValue, "protocol", translate("Protocol"))
a:value("socks", translate("socks"))
a:value("http", translate("http"))
a:value("dokodemo-door", translate("dokodemo"))
a:value("vmess", translate("vmess"))
a.rmempty = false

a = s:option(Value, "port", translate("Port"))
a.datatype = "port"
a.rmempty = false

s = m:section(TypedSection, "out", translate("V2ray Outbound"))
s.addremove = true
s.anonymous = false

a = s:option(Flag, "enable", translate("Enabled"))
a.rmempty = false
a.enabled = "1"
a.disabled = "0"
a.default = a.disabled

a = s:option(Flag, "mux", translate("Mux"))
a.rmempty = false
a.enabled = "1"
a.disabled = "0"
a.default = a.disabled

a = s:option(Value, "tag", translate("Tag"))

a = s:option(ListValue, "protocol", translate("Protocol"))
a:value("vmess", translate("vmess"))
a:value("socks", translate("socks"))
a.default = "vmess"

a = s:option(Value, "address", translate("Address"))
a.datatype = "string"
a.rmempty = false

a = s:option(Value, "port", translate("Port"))
a.datatype = "port"
a.rmempty = false

a = s:option(Value, "id", translate("Id"))
a:depends("protocol", "vmess")
a.rmempty = false

a = s:option(Value, "alterid", translate("Alterid"))
a:depends("protocol", "vmess")
a.rmempty = false

a = s:option(ListValue, "security", translate("Security"))
a:depends("protocol", "vmess")
a:value("auto", translate("auto"))
a:value("none", translate("none"))
a:value("chacha20-poly1305", translate("chacha20-poly1305"))
a:value("aes-128-gcm", translate("aes-128-gcm"))
a.defalut = "auto"

a = s:option(ListValue, "network", translate("Network"))
a:value("tcp", translate("TCP"))
a:value("kcp", translate("mKCP"))
a:value("ws", translate("WebSocket"))
a:value("h2", translate("HTTP/2"))
a:value("ds", translate("DomainSocket"))
a:value("quic", translate("QUIC"))
a.default = "tcp"

-- TCP
a = s:option(ListValue, "htype", translate("Type"))
a.rmempty = true
a:depends("network", "tcp")
a:value("", translate("none"))
a:value("http", translate("http"))
a.default = ""


a = s:option(Value, "path", translate("Path"))
a.placeholder = "/"
a.rmempty = true
a:depends({network="tcp", htype="http"})
a:depends("network", "ws")
a:depends("network", "h2")
a:depends("network", "ds")

-- mKCP
a = s:option(Value, "mtu", translate("mtu"))
a.placeholder = "1350"
a.rmempty = true
a:depends("network", "kcp")

a = s:option(Value, "tti", translate("tti"))
a.placeholder = "20"
a.rmempty = true
a:depends("network", "kcp")

a = s:option(Value, "uplinkcapacity", translate("uplinkCapacity"))
a.placeholder = "5"
a.rmempty = true
a:depends("network", "kcp")

a = s:option(Value, "downlinkcapacity", translate("downlinkCapacity"))
a.placeholder = "20"
a.rmempty = true
a:depends("network", "kcp")

a = s:option(Flag, "congestion", translate("congestion"))
a.rmempty = true
a.enabled = "1"
a.disabled = ""
a.default = a.disabled
a:depends("network", "kcp")

a = s:option(Value, "readbufferSize", translate("readBufferSize"))
a.placeholder = "1"
a.rmempty = true
a:depends("network", "kcp")

a = s:option(Value, "writebufferSize", translate("writeBufferSize"))
a.placeholder = "1"
a.rmempty = true
a:depends("network", "kcp")

-- HTTP/2
a = s:option(Value, "host", translate("Host"), "eg. www.baidu.com")
a.rmempty = true
a:depends({network="tcp", htype="http"}) -- TCP
a:depends("network", "ws") -- WebSocket
a:depends("network", "h2")

-- QUIC
a = s:option(ListValue, "method", translate("Security"))
a:depends("network", "quic")
a:value("none", translate("none"))
a:value("aes-128-gcm", translate("aes-128-gcm"))
a:value("chacha20-poly1305", translate("chacha20-poly1305"))

a = s:option(Value, "key", translate("Key"))
a.rmempty = true
a:depends({network="quic",method="aes-128-gcm"})
a:depends({network="quic",method="chacha20-poly1305"})

a = s:option(ListValue, "type", translate("Type"))
a:depends("network", "kcp")
a:depends("network", "quic")
a:value("none", translate("none"))
a:value("srtp", translate("srtp"))
a:value("utp", translate("utp"))
a:value("wechat-video", translate("wechat-video"))
a:value("dtls", translate("dtls"))
a:value("wireguard", translate("wireguard"))

-- TLS
a = s:option(Flag, "tls", translate("TLS"))
a.rmempty = true
a.enabled = "1"
a.disabled = ""
a.default = a.disabled

a = s:option(Flag, "allowinsecure", translate("AllowInsecure"))
a:depends({tls="1"})
a.rmempty = true
a.enabled = "1"
a.disabled = ""
a.default = a.disabled

a = s:option(Value, "servername", translate("serverName"))
a:depends("tls", "1")
a.rmempty = true


-- LINK
s = m:section(TypedSection, "link", translate("V2ray Link"))
s.addremove = false
s.anonymous = false

a = s:option(Flag, "enable", translate("Enabled"))
a.rmempty = false
a.enabled = "1"
a.disabled = "0"
a.default = a.disabled

a = s:option(ListValue, "url_type", translate("Type"))
a:value("config", translate("config"))
a:value("link", translate("link"))
a.default = "link"

a = s:option(Value, "url", translate("url"))
a.rmempty = false

-- LOG
s = m:section(TypedSection, "log", translate("V2ray Log"))
s.addremove = false
s.anonymous = true

a = s:option(ListValue, "level", translate("Level"))
a:value("none", translate("none"))
a:value("error", translate("error"))
a:value("warning", translate("warning"))
a:value("info", translate("info"))
a:value("debug", translate("debug"))


return m
