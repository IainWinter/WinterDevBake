#include "CommentGeneration.h"
#include "Util.h"
#include "mariadb/conncpp.hpp"

#include <iostream>
#include <unordered_map>
#include <stack>

struct Comment 
{
    int id = 0;
    int pid = 0;
    std::string name;
    std::string text;
    uint64_t date;
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

        sql::PreparedStatement* query = con->prepareStatement("SELECT comments.id, parent_id, name, text, UNIX_TIMESTAMP(date) FROM comments INNER JOIN subjects ON comments.subject_id = subjects.id WHERE subject = ?");
        query->setString(1, subject.c_str());

        sql::ResultSet* result = query->executeQuery();

        while (result->next())
        {
            Comment comment;
            comment.id = result->getInt(1);
            comment.pid = result->getInt(2);
            comment.name = result->getString(3);
            comment.text = result->getString(4);
            comment.date = result->getInt64(5);
           
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
	out << "<div class='comment' id='comment-" << comment.id << "'>"
		"<h3 class='comment-name mark-section'>" << comment.name << "</h3>"
		"<p class='comment-text'>" << comment.text << "</p>"
		"<div class='comment-meta'>"
			"<span class='comment-reply fake-href' data-comment-id='" << comment.id << "'>Reply</span>"
			"<span class='comment-time'>" << comment.date << "</span>"
		"</div>"
	"</div>" << std::endl;
}

//void GenerateCommentChainInner(std::ostream& out, int pid, std::unordered_map<int, std::vector<const Comment*>> commentsByPID)
//{
//}


void GenerateCommentChain(std::ostream& out, const Comment* comment, std::unordered_map<int, std::vector<const Comment*>> commentsByPID)
{
    out << "<div class='comment-chain' id='comment-chain-" << comment->id << "'>" << std::endl;

    std::stack<const Comment*> q;
    q.push(comment);

    while (!q.empty())
    {
        const Comment* c = q.top();
        q.pop();

        //printf("%d\n", c->id);

        GenerateComment(out, *c); // root comment

        if (commentsByPID.count(c->id))
            for (const Comment* child : commentsByPID.at(c->id))
                q.push(child);
    }

    out << "</div>" << std::endl;
}

void GenerateCommentSection(std::ostream& out, std::unordered_map<int, std::vector<const Comment*>> commentsByPID)
{
    if (commentsByPID.count(0) == 0) // no comments
        return;

    for (const Comment* comment : commentsByPID.at(0))
    {
        if (commentsByPID.count(comment->id) > 0)                // this is a parent of other comments
            GenerateCommentChain(out, comment, commentsByPID);
        else
            GenerateComment(out, *comment);
    }
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
        byParent[comment.pid].push_back(&comment);

    for (auto& [pid, list] : byParent)
        std::sort(list.begin(), list.end(), 
            [](const Comment* a, const Comment* b) { return a->date > b->date; }
        );

    // group comments by parent chains

    GenerateCommentSection(out, byParent);

	return 0;
}