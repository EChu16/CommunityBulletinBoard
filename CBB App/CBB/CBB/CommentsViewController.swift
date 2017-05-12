//
//  CommentsViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/23/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class CommentsViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {
    
    var post_id = ""
    var community_name = ""
    var allCommentsDict = [Dictionary<String,String>]()
    var user_name = ""
    var user_id = ""
    var post_name = ""
    var poster_name = ""
    var post_title = ""
    var post_desc = ""
    var post_date = ""
    var post_time = ""
    var post_oid = ""
    var community_oid = ""
    
    let dateFormatter = DateFormatter()

    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var userInput: UITextView!
    
    @IBAction func sendComment(_ sender: UIButton) {
        let finalUserInput = userInput.text?.trimmingCharacters(in: .whitespaces)
        if(!finalUserInput!.isEmpty) {
            let parameters: Parameters = [
                "post_id": self.post_id,
                "comment_text": finalUserInput!
            ]
            // create post request
            let url = "https://code.engineering.nyu.edu:8080/comments/new"
            Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in    
                if (response.response?.statusCode == nil) {
                    print("Couldn't connect to server.")
                }
                else if (response.response?.statusCode==400) {
                    print(String(data: response.data!, encoding: String.Encoding.utf8))
                }
                else if (response.response?.statusCode==200) {
                    self.userInput.text! = ""
                    self.loadAllComments()
                }
            }
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        loadAllComments()
        setupTextField()
        
        userInput.layer.borderColor = UIColor(colorLiteralRed: 204.0/255.0, green: 204.0/255.0, blue: 204.0/255.0, alpha: 1.0).cgColor
        userInput.layer.borderWidth = 1.0
        userInput.layer.cornerRadius = 3.0
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func goBackToPost(_ sender: UIBarButtonItem) {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "ViewPostViewController") as! ViewPostViewController
        
        desController.name = self.poster_name
        desController.pTitle = self.post_title
        desController.pDesc = self.post_desc
        desController.postdate = self.post_date
        desController.posttime = self.post_time
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        
        desController.owner_id = self.post_oid
        desController.post_id = self.post_id
        desController.community_name = self.community_name
        desController.community_oid = self.community_oid
        
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }
    
    
    func showBottomOfTable() {
        if(self.allCommentsDict.count != 0) {
            let lastIndexPath = IndexPath(row: allCommentsDict.count-1, section: 0)
            self.tableView.scrollToRow(at: lastIndexPath, at: .bottom, animated: false)
        }
    }
    
    func setupMenu() {
        if (revealViewController() != nil) {
            menuButton.target = revealViewController()
            menuButton.action = #selector(SWRevealViewController.revealToggle(_:))
            revealViewController().rearViewRevealWidth = 275
            
            view.addGestureRecognizer(self.revealViewController().panGestureRecognizer())
        }
        customNavBar()
    }
    
    func customNavBar() {
        let navImg = #imageLiteral(resourceName: "nav-bar")
        
        self.navigationController?.navigationBar.setBackgroundImage(navImg, for: .default)
        
        navigationController?.navigationBar.tintColor = UIColor(colorLiteralRed: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
        navigationController?.navigationBar.titleTextAttributes = [NSForegroundColorAttributeName: UIColor.white, NSFontAttributeName: UIFont(name: "Aleo-Bold", size: 20.0)]

    }
    
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = self.post_name
    }
    
    func setupTextField() {
        userInput.text = "Type your comment here"
        userInput.textColor = UIColor.lightGray
    }
    
    func textViewDidBeginEditing(_ textView: UITextView) {
        if textView.textColor == UIColor.lightGray {
            textView.text = nil
            textView.textColor = UIColor.black
        }
    }
    
    func textViewDidEndEditing(_ textView: UITextView) {
        if textView.text.isEmpty {
            textView.text = "Type your comment here"
            textView.textColor = UIColor.lightGray
        }
    }

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return allCommentsDict.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "CommentTableViewCell") as! CommentTableViewCell
        cell.commentPoster.text! = allCommentsDict[indexPath.row]["cname"]!
        cell.commentBody.text! = allCommentsDict[indexPath.row]["cbody"]!
        let timestamp = allCommentsDict[indexPath.row]["timestamp"]
        
        let index = timestamp?.index((timestamp?.startIndex)!, offsetBy: (timestamp?.characters.count)! - 2)
        timestamp?.substring(to: index!)
        let ts = timestamp?.replacingOccurrences(of: "\n", with: "").trimmingCharacters(in: .whitespaces)
        
        // Timestamp formatting
        self.dateFormatter.dateFormat = "EEE MMM dd HH:mm:ss yyyy"
        self.dateFormatter.locale = Locale.init(identifier: "en_US")
        let dateObj = self.dateFormatter.date(from: ts!)
        self.dateFormatter.dateFormat = "MMM dd, yyyy"
        let date_formatted = self.dateFormatter.string(from: dateObj!)
        self.dateFormatter.dateFormat = "hh:mm"
        self.dateFormatter.timeStyle = .short
        let time_formatted = self.dateFormatter.string(from: dateObj!)
        
        cell.timestamp.text! = date_formatted + " "  + time_formatted
        return cell
    }
    
    func loadAllComments() {
        allCommentsDict.removeAll()
        // create post request
        let parameters: Parameters = [
            "post_id": self.post_id,
            ]
        let url = "https://code.engineering.nyu.edu:8080/comments/get"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                print("Couldn't connect to server.")
            }
            else if (response.response?.statusCode==400) {
                print ("hit")
                debugPrint(String(data: response.data!, encoding: String.Encoding.utf8))
                print(response)
            }
            else if (response.response?.statusCode==200) {
                guard let object = response.result.value else {
                    print("Error")
                    return
                }
                let json = JSON(object)
                if let allComments = json.array {
                    for comment in allComments {
                        let cname = comment["username"].string!
                        let cbody = comment["text"].string!
                        let timestamp = comment["timestamp"].string!
                        let dict = ["cname": cname, "cbody": cbody, "timestamp": timestamp] as! [String: String]
                        self.allCommentsDict.append(dict)
                    }
                }
                self.tableView.reloadData()
                self.showBottomOfTable()
            }
        }
    }
}
