systemctl disable nuk3gh0st.service
service nuk3gh0st stop
if[ -f /usr/sbin/semanage ]; then
	semanage permissive -d insmod_t
fi
rm /usr/lib/nuk3gh0st.startup
rm /lib/systemd/system/nuk3gh0st.service
