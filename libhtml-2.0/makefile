
INCLUDE = -I./ \

LIB = -L./ \


CPPFLAGS = -g -Wall $(INCLUDE) $(LIB)

CC = g++

all: clean htmlparser.a

objects = block_manager.o html_tree.o tag_def.o hash_dict.o safe-ctype.o  URL.o tree_visitor_for_title.o  tree_visitor_for_content.o html_parser.o  tree_visitor_for_link.o  utils.o entities.o


$(objects): %.o: %.cpp
	$(CC) -c  $(CPPFLAGS) $< -o $@

htmlparser.a:$(objects)
	ar -r htmlparser.a  $(objects) librecode.a libcharsetdetector.a

clean:
	rm *.o -f
