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
    std::string pname;
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

        sql::PreparedStatement* query = con->prepareStatement(
            "SELECT"
                " comments.id,"
                " comments.parent_id,"
                " comments.name,"
                " IF(parents.name IS NULL, '', parents.name),"
                " comments.text,"
                " UNIX_TIMESTAMP(comments.date)"
            " FROM comments"
            " LEFT JOIN comments AS parents ON (parents.id = comments.parent_id)"
            " INNER JOIN subjects ON comments.subject_id = subjects.id WHERE subject = ?;"
        );
        query->setString(1, subject.c_str());

        sql::ResultSet* result = query->executeQuery();

        while (result->next())
        {
            Comment comment;
            comment.id = result->getInt(1);
            comment.pid = result->getInt(2);
            comment.name = result->getString(3);
            comment.pname = result->getString(4);
            comment.text = result->getString(5);
            comment.date = result->getInt64(6);
           
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
    std::time_t time = (std::time_t)comment.date;
    std::tm utc_tm;
    gmtime_s(&utc_tm, &time);

    char buf[80];
    std::strftime(buf, sizeof(buf), "%B %d, %Y", &utc_tm);

    out << 
        "<div class='comment' id='comment-" << comment.id << "'>"
            "<h3 class='comment-name mark-section'>" 
                "<span class='comment-name-text'>" << comment.name << "</span>"
                "<span class='comment-name-reply'>" << (comment.pname.length() > 0 ? ", in reply to " + comment.pname : "") << "</span>"
            "</h3>"
        "<p class='comment-text'>" << comment.text << "</p>"
        "<div class='comment-meta'>"
            "<span class='comment-reply fake-href' data-comment-id='" << comment.id << "' data-comment-name='" << comment.name << "'>Reply</span>"
            "<span class='comment-time'>" << buf << "</span>"
        "</div>"
    "</div>" << std::endl;
}

void GenerateCommentChain(std::ostream& out, const Comment* comment, std::unordered_map<int, std::vector<const Comment*>> commentsByPID)
{
    out << "<div class='comment-chain draw-left-line' id='comment-chain-" << comment->id << "'>" << std::endl;

    std::stack<const Comment*> dfs;
    dfs.push(comment);

    while (!dfs.empty())
    {
        const Comment* c = dfs.top();
        dfs.pop();

        GenerateComment(out, *c);

        if (commentsByPID.count(c->id))
            for (const Comment* child : commentsByPID.at(c->id))
                dfs.push(child);
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