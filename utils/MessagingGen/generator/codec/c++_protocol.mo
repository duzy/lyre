template cxx_message_field
--------------------------
.   if .type eq 'string'
    std::$(.type) $(.name);
.elsif .type eq 'strings'
    std::list<std::$(.type)> $(.name);
.elsif .type eq 'uint8'
    uint8_t $(.name);
.elsif .type eq 'uint16'
    uint16_t $(.name);
.elsif .type eq 'uint32'
    uint32_t $(.name);
.elsif .type eq 'int8'
    int8_t $(.name);
.elsif .type eq 'int16'
    int16_t $(.name);
.elsif .type eq 'int32'
    int32_t $(.name);
.end
-----------------------end

template cxx_protocol
---------------------
namespace $(.name)
{
    namespace messages
    {
.if +message < 256 ;
        typedef uint8_t tag_value_base;
.else ;
        typedef uint16_t tag_value_base;
.end ;

        enum class tag : tag_value_base
        {
.for message ;
            $(.name),
.end ;
        };

.for message ;
        struct $(.name)
        {
            constexpr static tag TAG = tag::$(.name);
.  for ->field ;
            $(str cxx_message_field)
.  end ;
        };

.end ;

        template < class P, class accessor >
        struct codec
        {
            typedef messages::tag_value_base        tag_value_base;
            typedef messages::tag                   tag;

.for message ;
            static std::size_t size (P *p, const $(.name) & m)
            {
.  var $fields = +->field
.  if $fields == 0 ;
                return 0;
.  elsif $fields == 1 ;
                return accessor::field_size (p, m.$(->field[0].name));
.  else ;
                return accessor::field_size (p, m.$(->field[0].name))
.    for slice(->field, 1, $fields-2) ;
                   +   accessor::field_size (p, m.$(.name))
.    end ;
                   +   accessor::field_size (p, m.$(->field[$fields-1].name));
.  end ;
            }
            static void encode (P *p, const $(.name) & m)
            {
.  for ->field ;
                accessor::put (p, m.$(.name));
.  end ;
            }
            static void decode (P *p, const $(.name) & m)
            {
.  for ->field ;
                accessor::get (p, m.$(.name));
.  end ;
            }

.end ;
        };

    }
}
