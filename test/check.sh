#!/bin/sh

u=Evanok
p=evangelius

#env

green='\e[0;32m'
red='\e[0;31m'
NC='\e[0m' # No Color
bold='\033[1m'
nobold='\033[0m'
binary=t411-daemon
here=`pwd`
log=/tmp/log
i=1

print_success ()
{
    echo -e "${bold}${green}\t$1${NC}${nobold}"
}

print_failed ()
{
    echo -e "${bold}${red}\t$1${NC}${nobold}"
}

# Check that t411 daemon is always alive
is_alive ()
{
    ps aux | grep -v grep | grep ${binary} >/dev/null
    if [ $? -eq 0 ]; then
	return "1";
    else
	return "0";
    fi
}

# Check t411
process_t411_test ()
{
    # $1 label of test
    # $2 result expected
    # $3 output expected

    echo "Test $i : $1"
    ./bin/${binary} 2>${log} 1>&2
    sleep 2
    is_alive
    if [ $? -eq $2 ]; then
	print_success "Return code : OK"
    else
	print_failed "Return code : KO"
    fi

    cat ${log} | grep "$3" >/dev/null
    if [ $? -eq 0 ]; then
	print_success "Expected output : OK"
    else
	print_failed "Expected output : KO"
    fi

    killall -q -9 ${binary}
    # uncomment next line to debug suit test
    cat $log && echo ""
    rm -f ${log}
    i=$(($i + 1))
}

killall -q -9 ${binary}

# Backup existing conf file
if [ -f /etc/${binary}.conf ]; then
    sudo chmod 777 /etc/${binary}.conf
    sudo chown $USER /etc/${binary}.conf
    echo "Create backup file of t411 daemon"
    cp /etc/${binary}.conf /tmp/${binary}.conf.old
fi


# Test on user configuration dation
sudo rm /etc/${binary}.conf
process_t411_test "No config file" 0 "does not exist"
sudo touch /etc/${binary}.conf
process_t411_test "Bad permission on config file" 0 "Permission denied"
sudo chmod 777 /etc/${binary}.conf
sudo chown $USER /etc/${binary}.conf
process_t411_test "Empty config file" 0 "Not able to get username"
echo "username TUTU" > /etc/${binary}.conf
process_t411_test "Only username in config file" 0 "Not able to get username"
echo "password TUTU" > /etc/${binary}.conf
process_t411_test "Only password in config file" 0 "Not able to get username"
echo "username titi" > /etc/${binary}.conf
echo "password tutu" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
process_t411_test "Incorrect username and password in config file" 0 "Wrong password"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
process_t411_test "Correct username and password in config file" 1 "uid\":\"94588399"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
process_t411_test "Missing email info in config file" 0 "Not able to get mail"
echo "# comment" >> /etc/${binary}.conf
echo "   username     ${u}   " > /etc/${binary}.conf
echo "# comment" >> /etc/${binary}.conf
echo "password		${p}" >> /etc/${binary}.conf
echo "# comment" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
process_t411_test "Trailing whitespace in config file" 1 "uid\":\"94588399"
echo "tutu username ${u}" > /etc/${binary}.conf
echo "password ${p} tutu" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
process_t411_test "Unexpected token in config file" 0 "Unknow key and data in config file"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tututata.fr" >> /etc/${binary}.conf
process_t411_test "Wrong format on mail info in config file" 0 "Wrong format for mail"


# Test on torrent info from config file
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "Y G zfzfzfzef" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Unknow key and data in config file"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "T zfzfzfzef" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Error during parsing torrent process"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "T G zfzfzfzef" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Error during parsing torrent process"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "T Gugu zfzfzfzef" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Error during parsing torrent process"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "T A zfzfzfzef" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Error during parsing torrent process"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "T S zfzfzfzef" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Error during parsing torrent process"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "T A zfzfzfzef" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Error during parsing torrent process"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "     T S suits 3 23 COUCOU" >> /etc/${binary}.conf
process_t411_test "Wrong format for torrent info in config file" 0 "Error during parsing torrent process"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "     T S suits 3 23" >> /etc/${binary}.conf
process_t411_test "Trailing whitespace before torrent info" 1 "uid\":\"94588399"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "     T      S suits 3 23" >> /etc/${binary}.conf
echo "T A	 attaque_des_titans 1 25	" >> /etc/${binary}.conf
echo "     T S breaking_bad		10 23    " >> /etc/${binary}.conf
process_t411_test "Multiple torrent info" 1 "uid\":\"94588399"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
./bin/${binary} 2>/dev/null 1>&2
process_t411_test "Multiple instance of t411 daemon" 1 "only one instance of t411 daemon can be run"
echo "username ${u}" > /etc/${binary}.conf
echo "password ${p}" >> /etc/${binary}.conf
echo "mail tutu@tata.fr" >> /etc/${binary}.conf
echo "T S suits 3 1" >> /etc/${binary}.conf
echo "T S suits 3 2" >> /etc/${binary}.conf
echo "T S suits 3 3" >> /etc/${binary}.conf
echo "T S suits 3 4" >> /etc/${binary}.conf
echo "T S suits 3 5" >> /etc/${binary}.conf
echo "T S suits 3 6" >> /etc/${binary}.conf
echo "T S suits 3 7" >> /etc/${binary}.conf
echo "T S suits 3 8" >> /etc/${binary}.conf
echo "T S suits 3 9" >> /etc/${binary}.conf
echo "T S suits 3 10" >> /etc/${binary}.conf
echo "T S suits 3 11" >> /etc/${binary}.conf
process_t411_test "More than 10 torrents info" 1 "uid\":\"94588399"

# Restore config fil
if [ -f /tmp/${binary}.conf.old ]; then
    echo "Restore t411 daemon config file"
    sudo cp /tmp/${binary}.conf.old /etc/${binary}.conf
else
    echo "Remove t411 daemon config file created for testing purpose"
    sudo rm /etc/${binary}.conf.old
fi
