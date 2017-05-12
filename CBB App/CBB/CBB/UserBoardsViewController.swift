//
//  UserBoardsViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/18/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class UserBoardsViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {
    
    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var rightBarItem: UIBarButtonItem!
    @IBOutlet weak var allUserBoards: UITableView!
    
    var user_name = ""
    var user_id = ""
    
    var userBoards = [Dictionary<String,String>]()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        loadUserBoards()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = "Joined Boards"
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
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
        self.navigationController?.navigationBar.titleTextAttributes = [NSForegroundColorAttributeName: UIColor.white, NSFontAttributeName: UIFont(name: "Aleo-Bold", size: 20.0)]
        
        
    }
    
    func loadUserBoards() {
        // create post request
        let url = "https://code.engineering.nyu.edu:8080/cbb/home"
        Alamofire.request(url, method: .post, encoding: JSONEncoding.default).responseJSON { response in
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
                var counter : Int = 1;
                if var allBoards = json.array {
                    let user_info = allBoards.removeLast()
                    self.user_name = user_info["username"].string!
                    self.user_id = (user_info["_id"].dictionary?["$oid"]?.string!)!
                    for board in allBoards {
                        let community_name = board["community_name"].string!
                        let num = String(counter) + "."
                        let dict = ["num": num, "community_name": community_name] as[String: String]
                        self.userBoards.append(dict)
                        counter += 1
                    }
                }
                self.allUserBoards.reloadData()
            }
        }
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return userBoards.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "UserBoardTableViewCell") as! UserBoardTableViewCell
        
        cell.boardNum.text! = userBoards[indexPath.row]["num"]!
        cell.boardName.text! = userBoards[indexPath.row]["community_name"]!
        
        if (indexPath.row % 2 == 1) {
            cell.cellView.backgroundColor = UIColor(colorLiteralRed: 245.0/255.0, green: 230.0/255.0, blue: 203.0/255.0, alpha: 1.0)
        }
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let revealViewController:SWRevealViewController = self.revealViewController()
        
        let cell:UserBoardTableViewCell = tableView.cellForRow(at: indexPath) as! UserBoardTableViewCell
        
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "CBBViewController") as! CBBViewController
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        desController.community_name = cell.boardName.text!
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)

    }
}
