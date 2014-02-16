#!/usr/bin/ruby -w

=begin
http://de.wikipedia.org/wiki/XPath
http://www.germane-software.com/software/rexml/docs/tutorial.html
=end

require 'rexml/document'
require 'json'
include REXML

xmlfile = File.new("xml/xml/message_8h.xml")
xmldoc = Document.new(xmlfile)

root = xmldoc.root

methods = []

XPath.each( root, "//memberdef[@kind='function']")  do |element|
  # namespace message_

  method = {}

	XPath.each(element, "name") do |el_name|
		method[:name] = el_name.text
  end

  method[:params] = []

  XPath.each(element, "param") do |param|
    param_values = {}
    param_values[:name] = XPath.first(param, "declname").text
    type = XPath.first(param, "type");
    param_values[:simple_type] =  type.text
    ref = XPath.first(type, ref)
    param_values[:ref] = ref.text if (ref)

    method[:params] << param_values
  end

  XPath.each(element, "parameteritem") do |el_params|
    name = XPath.first(el_params, "parametername").text
    method[:params][name.to_sym][:description] = XPath.first(el_params, "parameterdescription")
  end

  methods << method;
end


api = {
    operations: methods,
}

puts JSON.pretty_generate(methods)
