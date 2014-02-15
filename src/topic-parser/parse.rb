#!/usr/bin/env ruby
require 'uri'
require 'cgi'
topic = File.read(ARGV[0])
puts topic

1000000.times do
	uri = URI.parse(topic)
	CGI::parse(uri.query) if uri.query
end
