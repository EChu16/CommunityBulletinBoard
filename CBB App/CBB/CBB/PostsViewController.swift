//
//  PostsViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/22/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class PostsViewController: UIViewController {
    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var addPostBtn: UIBarButtonItem!
    @IBOutlet weak var scrollView: UIView!
    @IBOutlet weak var heightConstraint: NSLayoutConstraint!
    @IBOutlet weak var actualScrollView: UIScrollView!
    
    var community_name = ""
    var community_oid = ""
    var user_name = ""
    var user_id = ""
    var centerPosts = 0
    let post_width = 336
    let post_height = 296
    let space_between_posts = 20
    
    var nextPostY = 20
    let screenWidth = UIScreen.main.bounds.size.width
    let screenHeight = UIScreen.main.bounds.size.height
    let dateFormatter = DateFormatter()

    
    var allPostsDict = [Dictionary<String,String>]()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        loadPostsForCommunity()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
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
        self.navigationItem.title = community_name + " Posts"
        self.addPostBtn.image = UIImage(named: "add-post-icon")
    }
    
    func createSubView(oid: String, pid: String, pts: String, pt: String, pd: String, pn: String) {
        let subView:PostView = PostView(frame: CGRect(x:0, y:self.nextPostY, width: self.post_width, height: self.post_height))
        subView.owner_id = oid
        subView.post_id = pid
        subView.timestamp = pts
        subView.title = pt
        subView.desc = pd
        subView.poster_name = pn
        subView.user_name = self.user_name
        subView.user_id = self.user_id
        subView.controller = self
        subView.community_name = self.community_name
        subView.community_oid = self.community_oid
        subView.postTitle.text! = pt
        subView.postDesc.text! = pd
        // Timestamp formatting
        self.dateFormatter.dateFormat = "EEE MMM dd HH:mm:ss yyyy"
        self.dateFormatter.locale = Locale.init(identifier: "en_US")
        let dateObj = self.dateFormatter.date(from: pts)
        self.dateFormatter.dateFormat = "MMM dd, yyyy"
        subView.postDate = self.dateFormatter.string(from: dateObj!)
        self.dateFormatter.dateFormat = "hh:mm"
        self.dateFormatter.timeStyle = .short
        subView.postTime = self.dateFormatter.string(from: dateObj!)
        
        self.scrollView.addSubview(subView)
        subView.center = CGPoint(x: self.view.bounds.midX, y:subView.center.y)
        self.nextPostY += self.post_height + self.space_between_posts
        self.extendViewIfNeeded()
    }
    
    func extendViewIfNeeded() {
        let yPos = CGFloat(self.nextPostY)
        if(yPos >= self.screenHeight) {
            heightConstraint.constant = yPos
        }
    }
    
    @IBAction func addPost(_ sender: Any) {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "AddPostViewController") as! AddPostViewController
        desController.community_name = self.community_name
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        desController.community_oid = self.community_oid

        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }


    
    func loadPostsForCommunity() {
        // create post request
        let parameters: Parameters = [
            "community_name": community_name,
            ]
        let url = "https://code.engineering.nyu.edu:8080/posts/get"
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
                if let allPosts = json.array {
                    for post in allPosts {
                        let title = post["title"].string!
                        let desc = post["body"].string!
                        let postername = post["username"].string!
                        let oid = post["user_id"].dictionary?["$oid"]!.string!
                        let pid = post["_id"].dictionary?["$oid"]!.string!
                        var timestamp = post["timestamp"].string!
                        let index = timestamp.index(timestamp.startIndex, offsetBy: timestamp.characters.count - 2)
                        timestamp.substring(to: index)
                        let ts = timestamp.replacingOccurrences(of: "\n", with: "").trimmingCharacters(in: .whitespaces)
                        
                        let dict = ["postTitle": title, "postDesc": desc, "posterName": postername, "ownerID": oid, "timestamp": ts, "post_id": pid] as! [String: String]
                        self.allPostsDict.append(dict)
                        self.createSubView(oid: oid!, pid: pid!, pts: ts, pt: title, pd: desc, pn: postername)
                    }
                }
            }
        }
    }
}
