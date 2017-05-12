//
//  UserListViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/23/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class UserListViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {
    
    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var tableView: UITableView!
    
    var community_name = ""
    var community_desc = ""
    var community_oid = ""
    var user_name = ""
    var user_id = ""
    var allUsers = [Dictionary<String,String>]()

    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        fetchAllUsers()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func backToCBBView(_ sender: UIBarButtonItem) {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "CBBViewController") as! CBBViewController
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        desController.community_name = self.community_name
        desController.community_desc = self.community_desc
        desController.community_oid = self.community_oid
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
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
        self.navigationItem.title = community_name + " Users"
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return allUsers.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "UserListTableViewCell") as! UserListTableViewCell
        
        cell.username.text! = allUsers[indexPath.row]["num"]! + allUsers[indexPath.row]["username"]!
        return cell
    }

    
    func backToCommunityView() {
        // TODO: segue back to community view
    }
    
    func fetchAllUsers() {
        // create post request
        let parameters: Parameters = [
            "community_name": community_name,
            ]
        let url = "https://code.engineering.nyu.edu:8080/cbb/userlist"
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
                print (json)
                var counter : Int = 1;
                if let allUsers = json.array {
                    for user in allUsers {
                        let num = String(counter) + ". "
                        let uname = user["username"].string!
                        
                        let dict = ["num": num, "username": uname] as! [String: String]
                        self.allUsers.append(dict)
                        counter += 1
                    }
                }
                self.tableView.reloadData()
            }
        }
    }
}
