use codec

def load($xml) { lang XML in $xml }

for include {
    var $node = load('apps/protocol/'~.filename);
    .^.insert($node, $_);
}

var $protocol = str codec::cxx_protocol;
var $server   = str codec::cxx_server;
var $client   = str codec::cxx_client;

say('----------------------------')
say($protocol)
say('----------------------------')
say($server)
say('----------------------------')
say($client)
say('----------------------------')
