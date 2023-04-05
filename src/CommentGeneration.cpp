#include "CommentGeneration.h"
#include "Util.h"
#include "mariadb/conncpp.hpp"

#include <iostream>
#include <unordered_map>

struct Comment 
{
    int id = 0;
    int pid = 0;
    std::string name;
    std::string text;
    std::string date;
};

int GetCommentsForSubject(const std::string& subject, std::vector<Comment>* comments)
{
    comments->clear();

    try {
        sql::Driver* driver = sql::mariadb::get_driver_instance();
        sql::Connection* con = driver->connect("localhost:3306", "wbake", "wbake");

        if (!con->isValid())
            return panic(DATABASE_ERROR, "Failed to connect to database");

        con->setSchema("blog");

        sql::PreparedStatement* query = con->prepareStatement("SELECT comments.id, parent_id, name, text, date FROM comments INNER JOIN subjects ON comments.subject_id = subjects.id WHERE subject = ?");
        query->setString(1, subject.c_str());

        sql::ResultSet* result = query->executeQuery();

        while (result->next())
        {
            Comment comment;
            comment.id = result->getInt(1);
            comment.pid = result->getInt(2);
            comment.name = result->getString(3);
            comment.text = result->getString(4);
            comment.date = result->getString(5);

            comments->push_back(comment);
        }
    }

    catch (std::exception e)
    {
        comments->clear();
        return panic(DATABASE_ERROR, e.what());
    }

    return 0;
}

void GenerateComment(std::ostream& out, const Comment& comment)
{
	out << "<div class='comment' id='comment-${id}'>"
		"<h3 class='comment-name mark-section'>" << comment.name << "</h3>"
		"<p class='comment-text'>" << comment.text << "</p>"
		"<div class='comment-meta'>"
			"<span class='comment-reply fake-href' data-comment-id='" << comment.id << "'>Reply</span>"
			"<span class='comment-time'>" << comment.date << "</span>"
		"</div>"
	"</div>";
}

int GenerateComments(std::ostream& out, const std::string& subject)
{
    std::vector<Comment> comments;
    int err = GetCommentsForSubject(subject, &comments);

    if (err)
        return err;

    // find chains, only allow reply at the end of the chain

    std::unordered_map<int, std::vector<const Comment*>> byParent;

    for (const Comment& comment : comments)
    {
        byParent[comment.pid].push_back(&comment);
    }

    // group comments by parent chains

    for (Comment comment : comments)
        GenerateComment(out, comment);

	return 0;
}