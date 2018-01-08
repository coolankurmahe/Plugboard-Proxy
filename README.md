# Plugboard-Proxy
SSH was implemented using unblocking reads and writes with encryption from both end. The plugboard proxy adds an extra layer of encryption to connections towards TCP services. Instead of connecting directly to the service, clients connect to pbproxy (running on the same server), which then relays all traffic to the actual service
