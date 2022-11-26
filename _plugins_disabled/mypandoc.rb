module Jekyll
  module Converters
    class MyPandoc < Converter
      priority :high
      DEFAULT_EXTENSIONS = []
      DEFAULT_FORMAT = 'html5'
      TAG_OPEN = /^(\s*)\{\.([a-z]+)\s*(\.{3})?\}\s*(.*)'/m
      TAG_CLOSE = /^(.*)\{\/\}\s*$/m

      def initialize(config)
        Jekyll::External.require_with_graceful_fail "pandoc-ruby"

        @config = config
        @mode_key = "pandoc-document"
      end

      def convert(content)
        extensions = config_option('extensions', DEFAULT_EXTENSIONS)
        format = config_option('format', DEFAULT_FORMAT)

        new_content = ''
        newp = false
        tags = []
        tag_open = TAG_OPEN
        tag_close = TAG_CLOSE
        content.each_line do |line|
            o = tag_open.match(line)
            c = tag_close.match(line)
            if newp && o != nil
                new_content << o[1]
                new_content << "\n<div class=\""
                new_content << o[2]
                if o[3] != nil
                    new_content << " long"
                end
                new_content << "\">\\safeopenclass{"
                new_content << o[2]
                new_content << "}"
                new_contnet << o[4]
                tags.push(o[2])
            elsif c != nil
                if c[1] != nil
                    new_content << c[1]
                end
                new_content << "\\safecloseclass{"
                tag = tags.pop
                new_content << tag
                new_content << "}"
                new_content << "</div>"
            else
                new_content << content
                new_content << "\n"
            end
            newp = /^\s+$/.match(line) != nil
        end
        content = new_content

        content = PandocRuby.new(content, *extensions).send("to_#{format}")
        raise Erros::FatalException, "Conversion returned empty string" unless content.length > 0
        content
      end

      def config_option(key, default=nil)
        if @config[@mode_key]
          @config.fetch(@mode_key, {}).fetch(key, default)
        else
          default
        end
      end

      def matches(ext)
        ext =~ /^\.pandocmd$/i
      end

      def output_ext(ext)
        ".html"
      end
    end
  end
end

