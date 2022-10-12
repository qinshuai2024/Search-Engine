function Search() {
    // 1. 提取搜索数据，使用JQuery
    let query = $(".container .search input").val();
    console.log("query = " + query); // 用于调试
    // 2. 使用Ajax，发起http请求
    $.ajax({
        type: "GET",
        url: "/s?word=" + query,
        success: function(data){
            console.log(data);
            BuildHtml(data);
        }
    });
}

// 根据获得的json字符串构建搜索结果页面
// 三个标签 a, p, i
function BuildHtml(data) {
    let content = $(".container .content")
    // 清空历史搜索
    content.empty();
    for (let elem of data) {
        let a_lable = $("<a>", {
            text: elem.title,
            href: elem.url,
            // 跳转到新的页面
            target: "_blank"
        });

        let p_lable = $("<p>", {
            text: elem.content
        });
        let i_lable = $("<i>", {
            text: elem.url
        });
        let div_lable = $("<div>", {
            class: "item"
        });
        a_lable.appendTo(div_lable);
        p_lable.appendTo(div_lable);
        i_lable.appendTo(div_lable);
        div_lable.appendTo(content);
    }
}
let bg_img=["bgg1.jpg","bgg2.jpg","bgg3.jpg","bgg4.jpg"] 
document.getElementById("bg").style.backgroundImage="url("+bg_img[Math.floor(Math.random()*(bg_img.length))]+")";